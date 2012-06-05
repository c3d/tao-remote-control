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
#include <QtGlobal>
#include <QString>



Hook::Hook(int id)
    : id(id), command(""), execOnce(false)
// ----------------------------------------------------------------------------
//    Create hook
// ----------------------------------------------------------------------------
{
    IFTRACE(remotecontrol)
        debug() << "Hook created\n";
}


XL::Tree_p Hook::exec(XL::Context *context, XL::Tree_p self)
// ----------------------------------------------------------------------------
//    Execute XL code associated with this hook
// ----------------------------------------------------------------------------
{
    return evalInternal(context, self, (command != "") ? command : "false",
                        execOnce);
}


void Hook::setCommand(text cmd)
// ----------------------------------------------------------------------------
//    Set command to be run by this hook
// ----------------------------------------------------------------------------
{
    command = cmd;
    IFTRACE(remotecontrol)
        debug() << "XL code set to: " << command << "\n";
    // TODO: post event to request refresh
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