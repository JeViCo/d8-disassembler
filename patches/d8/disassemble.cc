void Shell::LoadBytecode(const v8::FunctionCallbackInfo<v8::Value>& info) {
    auto isolate = info.GetIsolate();
    auto isolateInternal = reinterpret_cast<v8::internal::Isolate*>(isolate);

    if (info.Length() < 1) {
        isolate->ThrowException(v8::Exception::Error(
            v8::String::NewFromUtf8(isolate, "No args found.").ToLocalChecked()));
        return;
    }

    v8::String::Utf8Value filename(isolate, info[0]);

    if (*filename == nullptr) {
        isolate->ThrowException(v8::Exception::Error(
            v8::String::NewFromUtf8(isolate, "Error creating filename.")
                .ToLocalChecked()));
        return;
    }

    std::ifstream file(*filename, std::ios::binary | std::ios::ate);

    if (!file) {
        isolate->ThrowException(v8::Exception::Error(
            v8::String::NewFromUtf8(isolate, "Error opening file.")
                .ToLocalChecked()));
        return;
    }

    const std::streamoff file_size = file.tellg();

    if (file_size < 0 || file_size > INT_MAX) {
        isolate->ThrowException(v8::Exception::Error(
            v8::String::NewFromUtf8(isolate, "Invalid bytecode file size.")
                .ToLocalChecked()));
        return;
    }

    const int length = static_cast<int>(file_size);
    std::unique_ptr<uint8_t[]> filedata(new uint8_t[length]);

    file.seekg(0, std::ios::beg);

    if (length > 0 &&
        !file.read(reinterpret_cast<char*>(filedata.get()), length)) {
        isolate->ThrowException(v8::Exception::Error(
            v8::String::NewFromUtf8(isolate, "Error reading file.")
                .ToLocalChecked()));
        return;
    }

#if V8_MAJOR_VERSION < 9
    v8::internal::ScriptData cached_data(filedata.get(), length);
#else
    v8::internal::AlignedCachedData cached_data(filedata.get(), length);
#endif

#if V8_MAJOR_VERSION < 9
    auto source =
        isolateInternal->factory()->NewStringFromAsciiChecked("source");
#else
    auto source =
        isolateInternal->factory()
            ->NewStringFromUtf8(v8::base::CStrVector("source"))
            .ToHandleChecked();
#endif

    printf("===== START DESERIALIZE BYTECODE =====\n");

#if V8_MAJOR_VERSION < 9
    v8::internal::CodeSerializer::Deserialize(
        isolateInternal,
        &cached_data,
        source,
        v8::ScriptOriginOptions());
#else
    v8::internal::ScriptDetails script_details;

    v8::internal::CodeSerializer::Deserialize(
        isolateInternal,
        &cached_data,
        source,
        script_details);
#endif
}
