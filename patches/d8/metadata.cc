void Shell::DumpOpcodes(const v8::FunctionCallbackInfo<v8::Value>& info) {
    using v8::internal::interpreter::Bytecode;
    std::cout << "{\"v8_version\":\"" << v8::V8::GetVersion() << "\",\"opcodes\":[";
    bool first = true;

#if V8_MAJOR_VERSION < 9
#define D8_DUMP_OPCODE_OLD(Name, AccumulatorUse, ...)              \
    do {                                                           \
        if (!first) std::cout << ',';                              \
        first = false;                                             \
        Bytecode bytecode = Bytecode::k##Name;                     \
        std::cout << "{\"name\":\"" << #Name << "\"";              \
        std::cout << ",\"value\":" << static_cast<int>(bytecode);  \
        std::cout << ",\"operands\":\"" << #__VA_ARGS__ << "\"";   \
        std::cout << "}";                                          \
    } while (false);
    BYTECODE_LIST(D8_DUMP_OPCODE_OLD)
#undef D8_DUMP_OPCODE_OLD
#else
#define D8_DUMP_OPCODE(Name, ...)                                 \
    do {                                                          \
        if (!first) std::cout << ',';                             \
        first = false;                                            \
        Bytecode bytecode = Bytecode::k##Name;                    \
        std::cout << "{\"name\":\"" << #Name << "\"";             \
        std::cout << ",\"value\":" << static_cast<int>(bytecode); \
        std::cout << ",\"operands\":\"" << #__VA_ARGS__ << "\"";  \
        std::cout << "}";                                         \
    } while (false);

    BYTECODE_LIST(D8_DUMP_OPCODE, D8_DUMP_OPCODE)
#undef D8_DUMP_OPCODE
#endif

    std::cout << "]}" << std::endl;
}
