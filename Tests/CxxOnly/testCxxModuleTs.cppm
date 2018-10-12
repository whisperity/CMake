export module testCxxModuleTs;

// Implementation detail of the module.
float Method() { return 3.0; }

export class CxxModuleTsClass {
public:
  static bool Test() { return Method() == 3.0; }
};
