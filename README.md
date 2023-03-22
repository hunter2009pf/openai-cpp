# OpenAI-Cpp
C++ library for calling OpenAI's api.
Only one OpenAIService class instance implements all network requests.
## Supported APIs
* [Models](https://platform.openai.com/docs/api-reference/models)
* [Completions](https://platform.openai.com/docs/api-reference/completions)
* [Chat](https://platform.openai.com/docs/api-reference/chat)
* [Edits](https://platform.openai.com/docs/api-reference/edits)
* [Images](https://platform.openai.com/docs/api-reference/images)
* [Embeddings](https://platform.openai.com/docs/api-reference/embeddings)
* [Audio(also known as whisper)](https://platform.openai.com/docs/api-reference/audio)
* [File](https://platform.openai.com/docs/api-reference/files)
* [Fine-tunes](https://platform.openai.com/docs/api-reference/fine-tunes)
* [Moderations](https://platform.openai.com/docs/api-reference/moderations)
## Compilation
The library currently supports Windows, Linux and Android platforms.
### Windows compilation
1. Install [vcpkg](https://vcpkg.io) tool, modify the path to vcpkg.make in openai-cpp/CMakeLists.txt;
`SET(CMAKE_TOOLCHAIN_FILE "absolute\\path\\to\\vcpkg.cmake")`
2. Download [openssl](https://github.com/openssl/openssl) source code and switch to tag "openssl-3.1.0"(maybe update to a higher version in the future).
    - install [perl](https://strawberryperl.com/)
    - install [nasm](https://www.nasm.us) and add nasm path to the environment variables list
    - open Visual Studio Developer Command Prompt with administrative privileges and find where vcvarsall.bat is.(you can use the commandline below and probably find it at "../visual_studio_2022/VC/Auxiliary/Build/")
  
        `find ../visual_studio_2022/ -name "vcvarsall*"`
    - query your windows SDK version(open Visual Studio Installer and find the installed SDK's version)
    - go into the directory where vcvarsall.bat locates and input the commandline into the prompt
  
        `vcvarsall.bat <your computer soc's architecture> <windows SDK version>`
    - From the root of the OpenSSL source directory enter
        perl Configure VC-WIN32 if you want 32-bit OpenSSL or
        perl Configure VC-WIN64A if you want 64-bit OpenSSL or
        perl Configure VC-WIN64-ARM if you want Windows on Arm (win-arm64) OpenSSL or
        perl Configure to let Configure figure out the platform
    - input the commandlines into the prompt
  
        `nmake`

        `nmake test`

        `nmake install`
3. Download [curl](https://github.com/curl/curl.git) source code and switch to tag "curl-8_0_1"(maybe update to a higher version in the future).
    - `cd <curl root directory>`  
      `./buildconf.bat`  
      `cd winbuild`  
      `nmake /f Makefile.vc mode=static WITH_DEVEL=<path/to/your/installed/openssl/library> VC=14 WITH_SSL=static ENABLE_NGHTTP2=no ENABLE_SSPI=no ENABLE_IDN=no GEN_PDB=no ENABLE_WINSSL=no DEBUG=no MACHINE=<your computer soc's architecture>`
    - modify curl headers and library paths in openai-cpp/CMakeLists.txt
      `SET(CURL_INCLUDE_DIR "path/to/curl/headers")`  
      `target_link_libraries(openai-cpp "path/to/libcurl_a.lib")`
4. Install jsoncpp
   - vcpkg.exe install jsoncpp 
   (note: vcpkg's default supported architecture is x86, if you need install x86_amd64 arch packages，please modify enviroment variable like this)
   `set system environment variable VCPKG_DEFAULT_TRIPLET=x64-windows`
5. Open cmake gui and start to compile openai-cpp static library
   - config openai-cpp root directory as src and specify the path to CMakeLists.txt
   - configure->genereate->open project->compile
6. Create a project to test openai-cpp.lib
   - add header dependencies:  
     include/openai  
     include/openai/models  
     include/openai/utils  
     curl headers  
     openssl headers
   - add library dependencies:  
     libcrypto.lib  
     libcurl_a.lib  
     libssl.lib  
     openai-cpp.lib
   - right click test project in Visual Studio, select properties, linker->input->add additional dependencies  
     Ws2_32.lib  
     winmm.lib  
     wldap32.lib  
     Crypt32.lib
   - in code implementation, set up SSL CA absolute path and enable using it, enable https proxy if necessary.
   - set up correct OpenAI API Key and Organization ID
   - run your test project~
### Linux compilation
TODO
### Android compilation
TODO