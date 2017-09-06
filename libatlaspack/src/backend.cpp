
#include <AtlasPack/Backend>

namespace AtlasPack {

#if 0
//https://msdn.microsoft.com/en-us/library/hh567368.aspx

/**
 * Helper function to ensure global variable instantiation is threadsafe,
 * even if the constructor itself is not
 */
template <typename T>
std::shared_ptr<T> globalStatic () {
    static std::shared_ptr<T> global;
    static boost::mutex m;

    //enter critical area
    boost::lock_guard<boost::mutex> guard(m);
    if(!global) {
        global = std::make_shared<T>();
    }
    return global;
}

typedef boost::container::list< std::shared_ptr<Backend> > BackendList;
auto factoryRegistry = globalStatic<BackendList>;

//the factory Mutex helper function itself does not need to use globalStatic,
//since the standard assures us only one thread can enter the mutex constructor
//all others will wait
boost::mutex &factoryMutex() {
    static boost::mutex m;
    return m;
}


std::shared_ptr<Backend> Backend::createBackendForFile(const std::string &path)
{
    std::shared_ptr<BackendList> factory = factoryRegistry();
}

std::shared_ptr<Backend> Backend::createBackendForExtension(const std::string &filetype)
{

}

bool Backend::registerBackend(std::shared_ptr<AtlasPack::BackendFactory> factory)
{

}

#endif

}
