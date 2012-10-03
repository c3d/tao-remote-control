// ****************************************************************************
//  hook.cpp                                                       Tao project
// ****************************************************************************
//
//   File Description:
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
// ****************************************************************************
// This software is property of Taodyne SAS - Confidential
// Ce logiciel est la propriété de Taodyne SAS - Confidentiel
//  (C) 2012 Jerome Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************


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
    : id(id), execOnce(false), onceCounter(0), refreshEvent(-1)
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
    text code = command();
    return evalInternal(context, self, (code != "") ? code : "false",
                        execOnce);
}


void Hook::setCommand(QString cmd, bool once)
// ----------------------------------------------------------------------------
//    Set command to be run by this hook
// ----------------------------------------------------------------------------
{
    // Call from main thread only, because ModuleApi::postEvent is not supposed
    // to be thread-safe. It also serializes access to this->cmd.
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    QMutexLocker locker(&mutex);

    execOnce = once;
    if (execOnce)
    {
        // Append a string that will differ each time a "once" command is set
        // See evalInternal()
        QString tag = QString(" /* %1 */").arg(onceCounter++);
        cmd += tag;
    }
    this->cmd = +cmd;
    IFTRACE(remotecontrolcode)
        debug() << "once=" << execOnce
                << " XL code set to: " << this->cmd << "\n";
    Q_ASSERT(tao);
    tao->postEventOnce(refreshEvent);
}


text Hook::command()
// ----------------------------------------------------------------------------
//    Return the current command (thread safe)
// ----------------------------------------------------------------------------
{
    QMutexLocker locker(&mutex);
    return text(cmd);
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
