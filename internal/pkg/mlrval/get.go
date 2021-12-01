package mlrval

import (
	"github.com/johnkerl/miller/internal/pkg/lib"
)

// It's essential that we use mv.Type() not mv.mvtype, or use an Is...()
// predicate, or another Get...(), since types are JIT-computed on first access
// for most data-file values. See type.go for more information.

func (mv *Mlrval) GetString() (stringValue string, isString bool) {
	if mv.Type() == MT_STRING || mv.Type() == MT_VOID {
		return mv.printrep, true
	} else {
		return "", false
	}
}

func (mv *Mlrval) GetIntValue() (intValue int, isInt bool) {
	if mv.Type() == MT_INT {
		return mv.intval, true
	} else {
		return -999, false
	}
}

func (mv *Mlrval) GetFloatValue() (floatValue float64, isFloat bool) {
	if mv.Type() == MT_FLOAT {
		return mv.floatval, true
	} else {
		return -777.0, false
	}
}

func (mv *Mlrval) GetNumericToFloatValue() (floatValue float64, isFloat bool) {
	if mv.Type() == MT_FLOAT {
		return mv.floatval, true
	} else if mv.Type() == MT_INT {
		return float64(mv.intval), true
	} else {
		return -888.0, false
	}
}

func (mv *Mlrval) GetNumericNegativeorDie() bool {
	floatValue, ok := mv.GetNumericToFloatValue()
	lib.InternalCodingErrorIf(!ok)
	return floatValue < 0.0
}

func (mv *Mlrval) GetBoolValue() (boolValue bool, isBool bool) {
	if mv.Type() == MT_BOOL {
		return mv.boolval, true
	} else {
		return false, false
	}
}

func (mv *Mlrval) GetArray() interface{} {
	if mv.IsArray() {
		return mv.arrayval
	} else {
		return nil
	}
}

func (mv *Mlrval) GetMap() interface{} {
	if mv.IsMap() {
		return mv.mapval
	} else {
		return nil
	}
}

func (mv *Mlrval) GetFunction() interface{} {
	if mv.Type() == MT_FUNC {
		return mv.funcval
	} else {
		return nil
	}
}
