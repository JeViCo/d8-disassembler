void Shell::DumpOpcodes(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using v8::internal::interpreter::Bytecode;

    std::cout << "{\"v8_version\":\""
              << v8::V8::GetVersion()
              << "\",\"opcodes\":[";

    bool first = true;

#if V8_MAJOR_VERSION < 9

#define D8_DUMP_OPCODE(Name, ...)                                  \
    do {                                                           \
        if (!first) std::cout << ',';                              \
        first = false;                                             \
        const Bytecode bytecode = Bytecode::k##Name;               \
        std::cout << "{\"name\":\"" << #Name << "\"";              \
        std::cout << ",\"value\":" << static_cast<int>(bytecode);  \
        std::cout << ",\"operands\":\"" << #__VA_ARGS__ << "\"}";  \
    } while (false);

    BYTECODE_LIST(D8_DUMP_OPCODE)

#undef D8_DUMP_OPCODE

#else

#define D8_DUMP_OPCODE(Name, ...)                                  \
    do {                                                           \
        if (!first) std::cout << ',';                              \
        first = false;                                             \
        const Bytecode bytecode = Bytecode::k##Name;               \
        std::cout << "{\"name\":\"" << #Name << "\"";              \
        std::cout << ",\"value\":" << static_cast<int>(bytecode);  \
        std::cout << ",\"operands\":\"" << #__VA_ARGS__ << "\"}";  \
    } while (false);

    BYTECODE_LIST(D8_DUMP_OPCODE, D8_DUMP_OPCODE)

#undef D8_DUMP_OPCODE

#endif

    std::cout << "]}" << std::endl;
}
