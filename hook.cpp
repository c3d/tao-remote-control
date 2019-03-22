// *****************************************************************************
// hook.cpp                                                        Tao3D project
// *****************************************************************************
//
// File description:
//
//    Execute commands sent from remote clients.
//
//
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2012, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2012, Catherine Burvelle <catherine@taodyne.com>
// (C) 2012,2014-2015,2019, Christophe de Dinechin <christophe@dinechin.org>
// (C) 2012, Jérôme Forissier <jerome@taodyne.com>
// *****************************************************************************
// This file is part of Tao3D
//
// Tao3D is free software: you can r redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tao3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tao3D, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************


#include "hook.h"
#include "base.h" // IFTRACE
#include "runtime.h"
#include "tao/module_api.h"
#include "tao/tao_utf8.h"
#include <QtGlobal>
#include <QCoreApplication>
#include <QEvent>
#include <QMutexLocker>
#include <QString>
#include <QThread>


extern Tao::ModuleApi *tao;


using namespace Tao;


Hook::Hook(int id)
    : id(id), onceCounter(0), refreshEvent(-1)
// ----------------------------------------------------------------------------
//    Create hook
// ----------------------------------------------------------------------------
{
    refreshEvent = QEvent::registerEventType();
    IFTRACE(remotecontrol)
        debug() << "Hook created. refreshEvent=" << refreshEvent << ".\n";
}


XL::Tree_p Hook::exec(XL::Context *context, XL::Tree_p self)
// ----------------------------------------------------------------------------
//    Execute XL code associated with this hook
// ----------------------------------------------------------------------------
{
    Command code = command();
    return evalInternal(context, self, (code.cmd != "") ? code.cmd : "false",
                        code.once);
}


void Hook::setCommand(QString cmd, bool once)
// ----------------------------------------------------------------------------
//    Set command to be run by this hook
// ----------------------------------------------------------------------------
{
    // Call from main thread only, because ModuleApi::postEvent is not supposed
    // to be thread-safe. It also serializes access to this->cmd.
    XL_ASSERT(QThread::currentThread() == qApp->thread());
    QMutexLocker locker(&mutex);

    XL_ASSERT(tao);

    // "Once" commands are stored in a FIFO. Other commands overwrite the
    // previous one.
    if (once)
    {
        // Append a string that will differ each time a "once" command is set
        // See evalInternal()
        QString tag = QString(" /* %1 */").arg(onceCounter++);
        cmd += tag;
        onceCommands.append(+cmd);
        IFTRACE(remotecontrolcode)
            debug() << "'Once' command appended. XL code: " << +cmd << "\n";
        tao->postEvent(refreshEvent);
    }
    else
    {
        IFTRACE(remotecontrolcode)
            debug() << "cmd set, XL code: " << +cmd << "\n";
        this->cmd = +cmd;
        tao->postEventOnce(refreshEvent);
    }
}


Hook::Command Hook::commandPeek()
// ----------------------------------------------------------------------------
//    Return the next command to execute (thread safe)
// ----------------------------------------------------------------------------
{
    QMutexLocker locker(&mutex);
    if (onceCommands.isEmpty())
        return Command(text(cmd), false);
    return Command(onceCommands.first(), true);
}


Hook::Command Hook::command()
// ----------------------------------------------------------------------------
//    [Pop] and return the next command to execute (thread safe)
// ----------------------------------------------------------------------------
{
    QMutexLocker locker(&mutex);
    if (onceCommands.isEmpty())
        return Command(text(cmd), false);
    cmd = "";
    return Command(onceCommands.takeFirst(), true);
}


std::ostream & Hook::debug()
// ----------------------------------------------------------------------------
//   Convenience method to log with a common prefix
// ----------------------------------------------------------------------------
{
    std::cerr << "[Hook " << (void*)this << " #" << id << "] ";
    return std::cerr;
}


// ============================================================================
//
//    Static methods
//
// ============================================================================



struct SourceCodeInfo : XL::Info
// ----------------------------------------------------------------------------
//    Associate some XL code and tree to a source code node
// ----------------------------------------------------------------------------
{
    SourceCodeInfo() {}
    ~SourceCodeInfo() {}
    text       src;
    XL::Tree_p tree;
};


XL::Tree_p Hook::evalInternal(XL::Context *context, XL::Tree_p self, text code,
                              bool once)
// ----------------------------------------------------------------------------
//    Execute code given as source text
// ----------------------------------------------------------------------------
{
    SourceCodeInfo *cached = self->GetInfo<SourceCodeInfo>();
    if (!cached)
    {
        cached = new SourceCodeInfo();
        self->SetInfo<SourceCodeInfo>(cached);
    }
    bool changed = false;
    if (cached->src != code)
    {
        XL::Tree *tree = XL::xl_parse_text(code);
        if (!tree)
            return XL::xl_false;
        cached->src = code;
        if (XL::Symbols *syms = self->Symbols())
            tree->SetSymbols(syms);
        cached->tree = tree;
        changed = true;
    }
    if (once && !changed)
        return XL::xl_false;
    return context->Evaluate(cached->tree);
}


XL::Tree_p Hook::eval(XL::Context *context, XL::Tree_p self, text code)
// ----------------------------------------------------------------------------
//    (Primitive) execute code given as source text
// ----------------------------------------------------------------------------
{
    return evalInternal(context, self, code);
}


XL::Tree_p Hook::evalOnce(XL::Context *context, XL::Tree_p self, text code)
// ----------------------------------------------------------------------------
//    (Primitive) execute code given as source text only once
// ----------------------------------------------------------------------------
{
    return evalInternal(context, self, code, true);
}
