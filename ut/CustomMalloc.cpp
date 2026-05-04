#include <dlfcn.h>
#include <unistd.h>


namespace CustomMalloc {

size_t mallocCounter_ = 0;

extern "C" {
    void* malloc(size_t size) noexcept {
        // This is the "assignment" part: find the real malloc once
        static auto real_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
        mallocCounter_++;        
        return real_malloc(size);
    }
}


size_t getMallocCounter() {
    return mallocCounter_;
}


void resetMallocCounter() {
    mallocCounter_ = 0;
}

}
