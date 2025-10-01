import pytest
import cmath
import functionparser

@pytest.mark.parametrize("datatype, input, expected", [
    ("d", 3, 9),
    ("f", 3, 9),
    ("ld", 3, 9),
    ("li", 3, 9),
    ("cd", 3+4j, -7+24j),
    ("cf", 3+4j, -7+24j),
    ("cld", 3+4j, -7+24j),
])
def test_FunctionParser_datatype(datatype, input, expected):
    fparser = getattr(functionparser, "FunctionParser_" + datatype)()
    fparser.Parse("x*x", "x")
    assert cmath.isclose(fparser.Eval([input]), expected)

def test_ParseAndDeduceVariables():
    fparser = functionparser.FunctionParser()
    assert fparser.ParseAndDeduceVariables("x*y") == ["x", "y"]

def test_AddFunction():
    f1 = functionparser.FunctionParser()
    f1.Parse("x*x*x", "x")
    f2 = functionparser.FunctionParser()
    f2.AddFunction("cb", f1)
    f2.Parse("cb(x)", "x")
    assert abs(f2.Eval([3]) - 27) < 1e-9

def test_AddConstant():
    fparser = functionparser.FunctionParser()
    fparser.AddConstant("y", 5)
    fparser.Parse("x*y", "x")
    assert abs(fparser.Eval([1.2]) - 6) < 1e-9

def test_SearchAndReplace():
    expression="2*child.id"
    searchAndReplacePairs = [("child.id", "safeid")]
    fparser = functionparser.FunctionParser()
    with pytest.raises(RuntimeError):
        fparser.ParseAndDeduceVariables(expression)
    fparser.ParseAndDeduceVariables(functionparser.SearchAndReplace(expression, searchAndReplacePairs))
    assert abs(fparser.Eval([5.5]) - 11) < 1e-9

def test_DivisionByZero():
    fparser = functionparser.FunctionParser()
    fparser.Parse("1/x", "x")
    assert abs(fparser.Eval([2]) - 0.5) < 1e-9
    with pytest.raises(RuntimeError):
        fparser.Eval([0])
