#include "PocoRequest.hpp"
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>

template <class HTTPSESSION>
static auto doRequest(HTTPSESSION &session, Poco::Net::HTTPRequest &request,
                      Poco::Net::HTTPResponse &response)
    -> Poco::JSON::Object::Ptr {
    session.sendRequest(request);
    std::istream &rs = session.receiveResponse(response);
    std::stringstream ss;
    Poco::StreamCopier::copyStream(rs, ss);

    /*int status = response.getStatus();
std::string reason = response.getReason();*/

    Poco::JSON::Object::Ptr ret;
    try {
        Poco::JSON::Parser parser;
        ret = parser.parse(ss).extract<Poco::JSON::Object::Ptr>();
    } catch (const std::exception &e) {
        std::cout << ss.str() << std::endl;
        std::cerr << e.what() << '\n';
    }

    return ret;
}

template <class HTTPSESSION>
static auto doRequestRaw(HTTPSESSION &session, Poco::Net::HTTPRequest &request,
                         Poco::Net::HTTPResponse &response)
    -> std::stringstream {
    session.sendRequest(request);
    std::istream &rs = session.receiveResponse(response);
    std::stringstream ss;
    Poco::StreamCopier::copyStream(rs, ss);

    return ss;
}

template <class HTTPSESSION>
static auto doRequestdebug(HTTPSESSION &session,
                           Poco::Net::HTTPResponse &response)
    -> Poco::JSON::Object::Ptr {
    std::istream &rs = session.receiveResponse(response);
    std::stringstream ss;
    Poco::StreamCopier::copyStream(rs, ss);

    std::cout << response.getStatus() << "   " << response.getReason()
              << std::endl;

    Poco::JSON::Object::Ptr ret;
    try {
        Poco::JSON::Parser parser;
        ret = parser.parse(ss).extract<Poco::JSON::Object::Ptr>();
    } catch (const std::exception &e) {
        std::cout << ss.str() << std::endl;
        std::cerr << e.what() << '\n';
    }

    return ret;
}

auto PocoRequest::http_post_json(const std::string &fullurl,
                                 const formdata_t &formdata)
    -> Poco::JSON::Object::Ptr {
    Poco::URI uri(fullurl);
    std::string path(uri.getPathAndQuery());
    if (path.empty()) {
        path = "/";
    }
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path,
                                   Poco::Net::HTTPMessage::HTTP_1_1);
    request.setContentType("application/json");

    Poco::Net::HTMLForm form;

    for (const auto &fm : formdata) {
        form.add(fm.first, fm.second);
    }

    form.prepareSubmit(request);

    if (fullurl.compare(0, 6, "https:") == 0) {
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());

        std::ostream &requestStream = session.sendRequest(request);
        form.write(requestStream);

        Poco::Net::HTTPResponse response;

        auto requestresult = doRequestdebug(session, response);
        int status = static_cast<int>(response.getStatus());

        if (status / 100 != 2) {
            std::cerr << "ERROR REQUEST HTTP:  " << fullurl << " " << status
                      << "   " << response.getReason() << std::endl;
        }

        return requestresult;
    }
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    std::ostream &requestStream = session.sendRequest(request);

    form.write(requestStream);

    Poco::Net::HTTPResponse response;

    auto requestresult = doRequestdebug(session, response);

    int status = static_cast<int>(response.getStatus());

    if (status / 100 != 2) {

        std::cerr << "ERROR REQUEST HTTP:  " << fullurl << " " << status

                  << "   " << response.getReason() << std::endl;
    }

    return requestresult;
}

auto PocoRequest::http_get_rawstream(const std::string &fullurl,
                                     const formdata_t &queryparams)
    -> std::stringstream {
    Poco::URI uri(fullurl);

    for (const auto &query : queryparams) {
        uri.addQueryParameter(query.first, query.second);
    }

    std::string path(uri.getPathAndQuery());
    if (path.empty()) {
        path = "/";
    }

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path,
                                   Poco::Net::HTTPMessage::HTTP_1_1);

    if (fullurl.compare(0, 6, "https:") == 0) {
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
        return doRequestRaw(session, request, responseraw);
    }
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    return doRequestRaw(session, request, responseraw);
}

auto PocoRequest::http_get_json(const std::string &fullurl,
                                const formdata_t &queryparams)
    -> Poco::JSON::Object::Ptr {
    Poco::URI uri(fullurl);

    for (const auto &query : queryparams) {
        uri.addQueryParameter(query.first, query.second);
    }

    std::string path(uri.getPathAndQuery());
    if (path.empty()) {
        path = "/";
    }

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path,
                                   Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse response;

    request.setContentType("application/json");

    if (fullurl.compare(0, 6, "https:") == 0) {
        Poco::Net::HTTPSClientSession session(uri.getHost(), uri.getPort());
        return doRequest(session, request, response);
    }
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

    return doRequest(session, request, response);
}
