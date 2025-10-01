template <typename T>
constexpr bool false_v = false;

#include <string.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <fparser.hh>

typedef std::pair<std::string, std::string> pss;
std::string SearchAndReplace(const std::string& in, const std::vector<pss>&_pairs);

template<typename Value_t>
struct FunctionParserWrapBase : FunctionParserBase<Value_t> {
    typedef FunctionParserBase<Value_t> BASE;
    void Parse(const std::string& Function, const std::string& Vars, bool useDegrees){
        int result = BASE::Parse(Function, Vars, useDegrees);
        if(result >= 0){
            throw std::runtime_error(std::string("Parse() error in character ")+std::to_string(result)+": "+BASE::ErrorMsg());
        }
    }
    Value_t Eval(const std::vector<typename BASE::value_type>& Vars){
        // todo: check the length of Vars (how?)
        Value_t result = BASE::Eval(Vars.data());
        int evalError = BASE::EvalError();
        if(evalError > 0){
            std::string evalErrorString;
            if(evalError == 1){
                evalErrorString = "division by zero";
            }else if(evalError == 2){
                evalErrorString = "sqrt error (sqrt of a negative value)";
            }else if(evalError == 3){
                evalErrorString = "log error (logarithm of a negative value)";
            }else if(evalError == 4){
                evalErrorString = "trigonometric error (asin or acos of illegal value)";
            }else if(evalError == 5){
                evalErrorString = "maximum recursion level in eval() reached";
            }else{
                evalErrorString = std::to_string(evalError);
            }
            throw std::runtime_error(std::string("Eval() error: ")+evalErrorString);
        }
        return result;
    }
    std::vector<std::string> ParseAndDeduceVariables(const std::string& Function, bool useDegrees){
        std::vector<std::string> resultVars;
        int result = BASE::ParseAndDeduceVariables(Function, resultVars, useDegrees);
        if(result >= 0){
            throw std::runtime_error(std::string("ParseAndDeduceVariables() error in character ")+std::to_string(result)+": "+BASE::ErrorMsg());
        }
        return resultVars;
    }
    bool AddFunction1(const std::string &name, typename BASE::FunctionPtr ptr, unsigned paramsAmount){
        return BASE::AddFunction(name, ptr, paramsAmount);
    }
    bool AddFunction2(const std::string &name, FunctionParserWrapBase<Value_t> &fp){
        return BASE::AddFunction(name, fp);
    }
};

template<typename Value_t>
void DefineFunctions(const pybind11::module_ &m, const std::string &suffix){
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<FunctionParserBase<Value_t>, std::shared_ptr<FunctionParserBase<Value_t>>>(m, ("__FunctionParserBase" + suffix).c_str());
    py::class_<FunctionParserWrapBase<Value_t>, FunctionParserBase<Value_t>, std::shared_ptr<FunctionParserWrapBase<Value_t>>> cFunctionParser(m, ("FunctionParser" + suffix).c_str());
    cFunctionParser.def(py::init<>());
    cFunctionParser.def("Parse", &FunctionParserWrapBase<Value_t>::Parse, "function"_a, "vars"_a, "useDegrees"_a=false);
    cFunctionParser.def("Eval", &FunctionParserWrapBase<Value_t>::Eval, "vars"_a);
    cFunctionParser.def("ParseAndDeduceVariables", &FunctionParserWrapBase<Value_t>::ParseAndDeduceVariables, "function"_a, "useDegrees"_a=false);
    // cFunctionParser.def("AddFunction", &FunctionParserWrapBase<Value_t>::AddFunction1, "name"_a, "ptr"_a, "paramsAmount"_a);  // todo
    cFunctionParser.def("AddFunction", &FunctionParserWrapBase<Value_t>::AddFunction2, "name"_a, "fp"_a);
 
    cFunctionParser.def("setDelimiterChar", &FunctionParserBase<Value_t>::setDelimiterChar, "delimiter"_a);
    cFunctionParser.def_property("epsilon", &FunctionParserBase<Value_t>::epsilon, &FunctionParserBase<Value_t>::setEpsilon);
    cFunctionParser.def("AddConstant", &FunctionParserBase<Value_t>::AddConstant, "name"_a, "value"_a);
    cFunctionParser.def("AddUnit", &FunctionParserBase<Value_t>::AddUnit, "name"_a, "value"_a);
    cFunctionParser.def("RemoveIdentifier", &FunctionParserBase<Value_t>::RemoveIdentifier, "name"_a);
    cFunctionParser.def("Optimize", &FunctionParserBase<Value_t>::Optimize);
    cFunctionParser.def("ForceDeepCopy", &FunctionParserBase<Value_t>::ForceDeepCopy);
}

PYBIND11_MODULE(functionparser, m){
    using namespace pybind11::literals;
    m.def("SearchAndReplace", &SearchAndReplace, "in"_a, "pairs"_a);

    DefineFunctions<double>(m, "_d");
    DefineFunctions<float>(m, "_f");
    DefineFunctions<long double>(m, "_ld");
    DefineFunctions<long>(m, "_li");

    DefineFunctions<std::complex<double>>(m, "_cd");
    DefineFunctions<std::complex<float>>(m, "_cf");
    DefineFunctions<std::complex<long double>>(m, "_cld");

    m.attr("FunctionParser") = m.attr("FunctionParser_d");
}

