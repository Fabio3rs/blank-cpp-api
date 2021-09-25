/**
 *@file CurlRAII.hpp
 * @author Fabio Rossini Sluzala ()
 * @brief RAII encapsulated cURL library functions
 * @version 0.1
 *
 *
 */
#pragma once
#ifndef CURL_RAII_HPP
#define CURL_RAII_HPP
#include <curl/curl.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <vector>

/**
 * @brief RAII curl CURL* deallocator curl_easy_cleanup
 */
class curlraii {
  public:
    void operator()(CURL *curl) { curl_easy_cleanup(curl); }
};

/**
 * @brief RAII curl curl_slist deallocator curl_slist_free_all
 */
class curlslitraii {
  public:
    void operator()(curl_slist *curl) { curl_slist_free_all(curl); }
};

/**
 * @brief RAII curl typedefs with unique_ptr
 */
typedef std::unique_ptr<CURL, curlraii> curlraii_t;
using curlslitraii_t = std::unique_ptr<curl_slist, curlslitraii>;

/**
 * @brief RAII curl callback class
 */
class curlCallBack {
  public:
    std::vector<int8_t> buffer;
    long response_code{0};
    CURLcode curl_perfm_res{CURLE_OK};
    bool storedata{true};

    /**
     * @brief CURL callback definition, if userp->storedata is false it will
     * store nothing
     * @param[in] data Input data
     * @param[in] size Size of the elements
     * @param[in] nmemb Number of elements
     * @param[in] userp User pointer, in this case curlCallBack*
     *
     * @return Size written in bytes
     */
    static auto cb(void *data, size_t size, size_t nmemb, curlCallBack *userp)
        -> size_t {
        size_t realsize = size * nmemb;

        if (userp->storedata) {
            userp->buffer.insert(
                userp->buffer.end(), reinterpret_cast<const char *>(data),
                reinterpret_cast<const char *>(data) + realsize);
        }

        return realsize;
    }

    curlCallBack() {}
};

class CurlRAII {
    CurlRAII();
    ~CurlRAII();

    CurlRAII(const CurlRAII &) = delete;
    CurlRAII(CurlRAII &&) = delete;

  public:
    /**
     * @brief similar to make_unique but with cURL curl_easy_init and curlraii_
     * @return curlraii_t object
     */
    static inline auto make_curl_easy() -> curlraii_t {
        return curlraii_t(curl_easy_init());
    }

    /**
     * @brief RAII version of curl_slist_append, create and append headers for
     * cURL
     * @param[out] ptr The curlslitraii_t variable to create/append
     */
    static inline void curl_slist_append_raii(curlslitraii_t &ptr,
                                              const char *str) {
        curl_slist *tmp = curl_slist_append(ptr.get(), str);

        if (tmp == nullptr) {
            throw std::runtime_error("Curl fail to run curl_slist_append");
        }

        if (!ptr) {
            ptr = curlslitraii_t(tmp);
        }

        /*
        cURL returns the first element from the curl_slist_append, that's what I
        am expecting in this function logic
        */
        if (ptr.get() != tmp) {
            throw std::logic_error(
                "curl curl_slist_append return ptr different");
        }
    }

    /**
     * @brief dispatch a workflow in github
     * @param[in] url The complete repository workflow dispatch
     * @param[in] GITHUBTOKEN Secret token from your account
     * @param[in] payload Json payload for your action
     *
     * @return curlCallBack with the curl result
     *
     * TODO: add a boolean to control the storage of the curlCallBack
     */
    static auto dispatch_github_action(const std::string &url,
                                       const std::string &GITHUBTOKEN,
                                       const std::string &payload)
        -> curlCallBack;

    static auto dispatch_common_action(const std::string &url,
                                       const std::string &ApiAccessID,
                                       const std::string &payload,
                                       int is_post = 1) -> curlCallBack;

    /**
     * @brief singleton of the CurlRAII class to call automatically
     * curl_global_init
     * @return CurlRAII& instance
     */
    static auto instance() -> CurlRAII &;
};

#endif
