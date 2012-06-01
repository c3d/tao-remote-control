// ****************************************************************************
//  remote_control.cpp                                             Tao project
// ****************************************************************************
// 
//   File Description:
// 
//    Functions control the Tao process remotely.
// 
// 
// 
// 
// 
// 
// 
// 
// ****************************************************************************
//  (C) 2012 Jérôme Forissier <jerome@taodyne.com>
//  (C) 2012 Taodyne SAS
// ****************************************************************************

#include "remote_control.h"
#include "tao/tao_utf8.h"
#include "tao/tao_info.h"
#include "tao/module_api.h"
#include "runtime.h"

using namespace Tao;


// Pointer to Tao functions
const Tao::ModuleApi *tao = NULL;


//static bool hasLicence()
//// ----------------------------------------------------------------------------
////   Check if we have a valid licence for this feature
//// ----------------------------------------------------------------------------
//{
//    static bool result = tao->checkImpressOrLicense("RemoteControl 1.0");
//    return result;
//}

struct SourceCodeInfo : XL::Info
// ----------------------------------------------------------------------------
//    Store XL code and tree
// ----------------------------------------------------------------------------
{
    SourceCodeInfo() {}
    ~SourceCodeInfo() {}
    text       src;
    XL::Tree_p tree;
};


XL::Tree_p evalInternal(XL::Context *context, XL::Tree_p self, text code,
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


XL::Tree_p eval(XL::Context *context, XL::Tree_p self, text code)
// ----------------------------------------------------------------------------
//    Execute code given as source text
// ----------------------------------------------------------------------------
{
    return evalInternal(context, self, code);
}


XL::Tree_p evalOnce(XL::Context *context, XL::Tree_p self, text code)
// ----------------------------------------------------------------------------
//    Execute code given as source text
// ----------------------------------------------------------------------------
{
    return evalInternal(context, self, code, true);
}


XL::Tree_p remoteControlHook(XL::Context */*context*/, XL::Tree_p /*self*/,
                             int /*id*/, int /*port*/)
// ----------------------------------------------------------------------------
//    Start server to execute commands
// ----------------------------------------------------------------------------
{
    return XL::xl_false;
}



// ============================================================================
// 
//    Module glue
// 
// ============================================================================

int module_init(const Tao::ModuleApi *api, const Tao::ModuleInfo *)
// ----------------------------------------------------------------------------
//   Initialize the Tao module
// ----------------------------------------------------------------------------
{
    XL_INIT_TRACES();
    tao = api;
    return 0;
}


int module_exit()
// ----------------------------------------------------------------------------
//   Uninitialize the Tao module
// ----------------------------------------------------------------------------
{
    return 0;
}



// Define traces for this modules
XL_DEFINE_TRACES

