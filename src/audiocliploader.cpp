#include "../include/audiocliploader.hpp"

    int getAudioType(std::string path) {
    if (path.ends_with(".ogg")) {
        return 0xE;
    } else if (path.ends_with(".wav")) {
        return 0x14;
    } else if (path.ends_with(".mp3")) {
        return 0xD;
    }
    return 0;
    }

bool audioClipLoader::loader::load()
{



    //TODO figure out how to get logging working here!
    //Stage 0 
    bool fileError = fileexists(filePath.c_str());
    bool error = (audioClipAsync != nullptr || audioSource != nullptr || !fileError);
    if(error)
    {
        if(!fileError)
        {
            
        } else 
        {
            
        }
        return false;
    }

    //Stage 1
    Il2CppString* filePathStr = il2cpp_utils::createcsstr("file:///" + filePath);


    audioType = getAudioType(filePath);
    audioClipRequest = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine.Networking", "UnityWebRequestMultimedia", "GetAudioClip", filePathStr, audioType));
    audioClipAsync = CRASH_UNLESS(il2cpp_utils::RunMethod(audioClipRequest, "SendWebRequest"));
    
    //Stage 2
    const MethodInfo* addCompletedMethod = CRASH_UNLESS(il2cpp_utils::FindMethodUnsafe(audioClipAsync, "add_completed", 1));
    auto action = CRASH_UNLESS(il2cpp_utils::MakeAction(addCompletedMethod, 0, this, audioClipCompleted));
    CRASH_UNLESS(il2cpp_utils::RunMethod(audioClipAsync, addCompletedMethod, action));
    return true;
}

void audioClipLoader::loader::audioClipCompleted(loader* obj, Il2CppObject* asyncOp)
{
    // Stage 1
    Il2CppObject* temporaryClip = CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine.Networking", "DownloadHandlerAudioClip", "GetContent", obj->audioClipRequest));
    if(temporaryClip != nullptr)
    {  
        Il2CppString* goName = il2cpp_utils::createcsstr("AudioClipGO");
        Il2CppObject* audioClipGO = CRASH_UNLESS(il2cpp_utils::NewUnsafe(il2cpp_utils::GetClassFromName("UnityEngine", "GameObject"), goName));
        obj->audioSource = CRASH_UNLESS(il2cpp_utils::RunMethod(audioClipGO, "AddComponent", il2cpp_utils::GetSystemType("UnityEngine", "AudioSource")));
        CRASH_UNLESS(il2cpp_utils::SetPropertyValue(obj->audioSource, "playOnAwake", false));
        CRASH_UNLESS(il2cpp_utils::SetPropertyValue(obj->audioSource, "clip", temporaryClip));
        CRASH_UNLESS(il2cpp_utils::RunMethod(audioClipGO, "DontDestroyOnLoad", audioClipGO));
        obj->loaded = true;
    }
    // Finished
}

Il2CppObject* audioClipLoader::loader::getClip()
{
    if(audioSource != nullptr && loaded)
    {
        return CRASH_UNLESS(il2cpp_utils::GetPropertyValue(audioSource, "clip"));
    } else
    {
        return nullptr;
    }
}


