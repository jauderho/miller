package mappers

import (
	"fmt"
	"os"

	"miller/clitypes"
	"miller/containers"
	"miller/mapping"
)

// ----------------------------------------------------------------
var NothingSetup = mapping.MapperSetup{
	Verb:         "nothing",
	ParseCLIFunc: mapperNothingParseCLI,
	UsageFunc:    mapperNothingUsage,
	IgnoresInput: false,
}

func mapperNothingParseCLI(
	pargi *int,
	argc int,
	args []string,
	_ *clitypes.TReaderOptions,
	__ *clitypes.TWriterOptions,
) mapping.IRecordMapper {
	*pargi += 1

	// xxx temp err keep or no
	mapper, _ := NewMapperNothing()
	return mapper
}

func mapperNothingUsage(
	o *os.File,
	argv0 string,
	verb string,
) {
	fmt.Fprintf(o, "Usage: %s %s [options]\n", argv0, verb)
	fmt.Fprintf(o, "Drops all input records. Useful for testing, or after tee/print/etc. have\n")
	fmt.Fprintf(o, "produced other output.\n")
}

// ----------------------------------------------------------------
type MapperNothing struct {
	// stateless
}

func NewMapperNothing() (*MapperNothing, error) {
	return &MapperNothing{}, nil
}

func (this *MapperNothing) Map(
	inrecAndContext *containers.LrecAndContext,
	outrecsAndContexts chan<- *containers.LrecAndContext,
) {
	if inrecAndContext.Lrec == nil { // end of stream
		outrecsAndContexts <- inrecAndContext
	}
}
