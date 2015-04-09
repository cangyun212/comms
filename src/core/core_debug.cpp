#include "core/core.hpp"
#include "core/core_debug.hpp"
#include "core/console/core_console_printer.hpp"
#include  "boost/format.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#if defined(CORE_DEBUG) && defined(CORE_COMPILER_GCC) && defined(CORE_PLATFORM_LINUX)
#include <execinfo.h>
#include <dlfcn.h>
#include <signal.h>
#include <cxxabi.h>
#include <ucontext.h>


#if defined(REG_RIP)
# define SIGSEGV_STACK_IA64
#elif defined(REG_EIP)
# define SIGSEGV_STACK_X86
#else
# define SIGSEGV_STACK_GENERIC
#endif

static void log_stacktrace(ucontext_t *ucontext, const std::string & notice)
{
    std::string string_to_print = std::string("\n") + notice;
    ucontext_t ucontext2;   

#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)
    int f = 0;
    Dl_info dlinfo;
    void **bp = 0;
    void *ip = 0;
#else
    void *bt[20];
    char **strings;
    size_t sz;
#endif

    // use an obtained context if none already supplied
    if (ucontext == NULL && getcontext(&ucontext2) == 0)
        ucontext = &ucontext2;

#if defined(SIGSEGV_STACK_X86) || defined(SIGSEGV_STACK_IA64)
    if (ucontext != NULL)
    {
# if defined(SIGSEGV_STACK_IA64)
        string_to_print +=std::string("SIGSEGV_STACK_IA64\n");
        ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];
        bp = (void**)ucontext->uc_mcontext.gregs[REG_RBP];
# elif defined(SIGSEGV_STACK_X86)
        string_to_print+=std::string("SIGSEGV_STACK_X86\n");
        ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];
        bp = (void**)ucontext->uc_mcontext.gregs[REG_EBP];
# endif
    }
    string_to_print += std::string("Stack trace:\n");
    while(bp && ip) 
    {
        if(!dladdr(ip, &dlinfo)) 
            break;

        const char *symname = dlinfo.dli_sname; 
#ifndef NO_CPP_DEMANGLE 
        int status;
        char *tmp = abi::__cxa_demangle(symname, NULL, 0, &status);

        if(status == 0 && tmp)
            symname = tmp;
#endif
        string_to_print += (boost::format("% 2d: %p <%s+%u>\n") %++f % ip % symname 
                %  (unsigned)((((char*)ip) - ((char*)dlinfo.dli_saddr)))).str() ;

#ifndef NO_CPP_DEMANGLE
        if(tmp)
            free(tmp);
#endif

        if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
            break;

        ip = bp[1];
        bp = (void**)bp[0];
    }
#else
    string_to_print+=std::string("SIGSEGV_STACK_GENERIC\n");
    string_to_print += std::string("Stack trace (non-dedicated):\n");
    sz = backtrace(bt, 20);
    int i = 0;
    strings = backtrace_symbols(bt, sz);
    for(i = 0; i < sz; ++i)
    {
        string_to_print += boost::format("%s\n") % strings[i] ;
    }
    free (strings);
#endif
    string_to_print += std::string( "End of stack trace\n---\n");
    if(sg::ConsolePrinter::Instance().IsReady())
    {    CORE_PRINT_DEBUG_NO_BLOCK(string_to_print);}
    else
        std::cerr<<string_to_print;
}

static void signal_segv(int signum, siginfo_t *info, void *ptr)  
{
    switch (signum)
    {
        case SIGILL:
            log_stacktrace((ucontext_t*)ptr, str(boost::format("SIGILL(%lu) Illegal Instruction!\n")% signum));
            break;
        case SIGBUS:
            log_stacktrace((ucontext_t*)ptr,str( boost::format("SIGBUS(%lu) Bus Error!\n") % signum));
            break;
        case SIGSEGV:
            log_stacktrace((ucontext_t*)ptr,str( boost::format("SIGSEGV(%lu) Segmentation Fault!\n")% signum));
            break;
        case SIGFPE:
            log_stacktrace((ucontext_t*)ptr,str( boost::format("SIGFPE(%lu) Floating Point Exception!\n") % signum));
            break;
        case SIGTERM:
            log_stacktrace((ucontext_t*)ptr,str( boost::format("SIGTERM(%lu) Program Terminated!\n") % signum));
            break;
        case SIGINT:
            log_stacktrace((ucontext_t*)ptr,str( boost::format("SIGINT(%lu) Program Interrupted!\n") % signum));
            break;
        case SIGABRT:
            log_stacktrace((ucontext_t*)ptr, str(boost::format("SIGABRT(%lu) Program Aborted!\n") % signum));
            break;
        case SIGUSR1:
            log_stacktrace((ucontext_t*)ptr,str( boost::format("SIGUSR1(%lu) Watchdog Abort!\n")% signum));
            break;
    }
    exit (-1); // really need quit program now.
}

static int reg_sigaction(int signum)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = signal_segv;
    action.sa_flags = SA_SIGINFO;
    if(sigaction(signum, &action, NULL) < 0) 
    {
        perror("sigaction");
        return 0;
    }
    return 1;
}

int setup_sig_segv(void)
{
    reg_sigaction(SIGSEGV);
    reg_sigaction(SIGILL);
    reg_sigaction(SIGBUS);
    reg_sigaction(SIGFPE);
    reg_sigaction(SIGTERM);
    reg_sigaction(SIGINT);
    reg_sigaction(SIGABRT);
    reg_sigaction(SIGUSR1);
    return 1;
}
#endif
