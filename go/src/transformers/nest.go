package transformers

import (
	"bytes"
	"container/list"
	"fmt"
	"os"
	"regexp"
	"strconv"
	"strings"

	"miller/src/cliutil"
	"miller/src/lib"
	"miller/src/transforming"
	"miller/src/types"
)

// ----------------------------------------------------------------
const verbNameNest = "nest"

var NestSetup = transforming.TransformerSetup{
	Verb:         verbNameNest,
	UsageFunc:    transformerNestUsage,
	ParseCLIFunc: transformerNestParseCLI,
	IgnoresInput: false,
}

func transformerNestUsage(
	o *os.File,
	doExit bool,
	exitCode int,
) {
	argv0 := lib.MlrExeName()
	verb := verbNameNest

	fmt.Fprintf(o, "Usage: %s %s [options]\n", argv0, verb)
	fmt.Fprintf(o, "Explodes specified field values into separate fields/records, or reverses this.\n")
	fmt.Fprintf(o, "Options:\n")
	fmt.Fprintf(o, "  --explode,--implode   One is required.\n")
	fmt.Fprintf(o, "  --values,--pairs      One is required.\n")
	fmt.Fprintf(o, "  --across-records,--across-fields One is required.\n")
	fmt.Fprintf(o, "  -f {field name}       Required.\n")
	fmt.Fprintf(o, "  --nested-fs {string}  Defaults to \";\". Field separator for nested values.\n")
	fmt.Fprintf(o, "  --nested-ps {string}  Defaults to \":\". Pair separator for nested key-value pairs.\n")
	fmt.Fprintf(o, "  --evar {string}       Shorthand for --explode --values ---across-records --nested-fs {string}\n")
	fmt.Fprintf(o, "  --ivar {string}       Shorthand for --implode --values ---across-records --nested-fs {string}\n")
	fmt.Fprintf(o, "Please use \"%s --usage-separator-options\" for information on specifying separators.\n",
		argv0)

	fmt.Fprintf(o, "\n")
	fmt.Fprintf(o, "Examples:\n")

	fmt.Fprintf(o, "\n")
	fmt.Fprintf(o, "  %s %s --explode --values --across-records -f x\n", argv0, verb)
	fmt.Fprintf(o, "  with input record \"x=a;b;c,y=d\" produces output records\n")
	fmt.Fprintf(o, "    \"x=a,y=d\"\n")
	fmt.Fprintf(o, "    \"x=b,y=d\"\n")
	fmt.Fprintf(o, "    \"x=c,y=d\"\n")
	fmt.Fprintf(o, "  Use --implode to do the reverse.\n")

	fmt.Fprintf(o, "\n")
	fmt.Fprintf(o, "  %s %s --explode --values --across-fields -f x\n", argv0, verb)
	fmt.Fprintf(o, "  with input record \"x=a;b;c,y=d\" produces output records\n")
	fmt.Fprintf(o, "    \"x_1=a,x_2=b,x_3=c,y=d\"\n")
	fmt.Fprintf(o, "  Use --implode to do the reverse.\n")

	fmt.Fprintf(o, "\n")
	fmt.Fprintf(o, "  %s %s --explode --pairs --across-records -f x\n", argv0, verb)
	fmt.Fprintf(o, "  with input record \"x=a:1;b:2;c:3,y=d\" produces output records\n")
	fmt.Fprintf(o, "    \"a=1,y=d\"\n")
	fmt.Fprintf(o, "    \"b=2,y=d\"\n")
	fmt.Fprintf(o, "    \"c=3,y=d\"\n")

	fmt.Fprintf(o, "\n")
	fmt.Fprintf(o, "  %s %s --explode --pairs --across-fields -f x\n", argv0, verb)
	fmt.Fprintf(o, "  with input record \"x=a:1;b:2;c:3,y=d\" produces output records\n")
	fmt.Fprintf(o, "    \"a=1,b=2,c=3,y=d\"\n")

	fmt.Fprintf(o, "\n")
	fmt.Fprintf(o, "Notes:\n")
	fmt.Fprintf(o, "* With --pairs, --implode doesn't make sense since the original field name has\n")
	fmt.Fprintf(o, "  been lost.\n")
	fmt.Fprintf(o, "* The combination \"--implode --values --across-records\" is non-streaming:\n")
	fmt.Fprintf(o, "  no output records are produced until all input records have been read. In\n")
	fmt.Fprintf(o, "  particular, this means it won't work in tail -f contexts. But all other flag\n")
	fmt.Fprintf(o, "  combinations result in streaming (tail -f friendly) data processing.\n")
	fmt.Fprintf(o, "* It's up to you to ensure that the nested-fs is distinct from your data's IFS:\n")
	fmt.Fprintf(o, "  e.g. by default the former is semicolon and the latter is comma.\n")
	fmt.Fprintf(o, "See also %s reshape.\n", argv0)

	if doExit {
		os.Exit(exitCode)
	}
}

func transformerNestParseCLI(
	pargi *int,
	argc int,
	args []string,
	_ *cliutil.TOptions,
) transforming.IRecordTransformer {

	// Skip the verb name from the current spot in the mlr command line
	argi := *pargi
	verb := args[argi]
	argi++

	// Parse local flags
	fieldName := ""
	nestedFS := ";"
	nestedPS := ":"
	doExplode := true
	doPairs := true
	doAcrossFields := true
	doExplodeSpecified := false
	doPairsSpecified := false
	doAcrossFieldsSpecified := false
	evfs := ""
	ivfs := ""

	for argi < argc /* variable increment: 1 or 2 depending on flag */ {
		opt := args[argi]
		if !strings.HasPrefix(opt, "-") {
			break // No more flag options to process
		}
		argi++

		if opt == "-h" || opt == "--help" {
			transformerNestUsage(os.Stdout, true, 0)

		} else if opt == "-f" {
			fieldName = cliutil.VerbGetStringArgOrDie(verb, opt, args, &argi, argc)

		} else if opt == "--explode" || opt == "-e" {
			doExplode = true
			doExplodeSpecified = true
		} else if opt == "--implode" || opt == "-i" {
			doExplode = false
			doExplodeSpecified = true

		} else if opt == "--values" || opt == "-v" {
			doPairs = false
			doPairsSpecified = true
		} else if opt == "--pairs" || opt == "-p" {
			doPairs = true
			doPairsSpecified = true

		} else if opt == "--across-fields" || opt == "-F" {
			doAcrossFields = true
			doAcrossFieldsSpecified = true
		} else if opt == "--across-records" || opt == "-R" {
			doAcrossFields = false
			doAcrossFieldsSpecified = true

		} else if opt == "--nested-fs" || opt == "-S" {
			nestedFS = cliutil.VerbGetStringArgOrDie(verb, opt, args, &argi, argc)
		} else if opt == "--nested-ps" || opt == "-P" {
			nestedPS = cliutil.VerbGetStringArgOrDie(verb, opt, args, &argi, argc)

		} else if opt == "--evar" {
			evfs = cliutil.VerbGetStringArgOrDie(verb, opt, args, &argi, argc)

		} else if opt == "--ivar" {
			ivfs = cliutil.VerbGetStringArgOrDie(verb, opt, args, &argi, argc)

		} else {
			transformerNestUsage(os.Stderr, true, 1)
		}
	}

	if evfs != "" {
		doExplode = true
		doPairs = false
		doAcrossFields = false
		nestedFS = evfs
	}
	if ivfs != "" {
		doExplode = false
		doPairs = false
		doAcrossFields = false
		nestedFS = ivfs
	}

	if fieldName == "" {
		transformerNestUsage(os.Stderr, true, 1)
	}
	if !doExplodeSpecified {
		transformerNestUsage(os.Stderr, true, 1)
	}
	if !doPairsSpecified {
		transformerNestUsage(os.Stderr, true, 1)
	}
	if !doAcrossFieldsSpecified {
		transformerNestUsage(os.Stderr, true, 1)
	}
	if doPairs && !doExplode {
		transformerNestUsage(os.Stderr, true, 1)
	}

	transformer, err := NewTransformerNest(
		fieldName,
		nestedFS,
		nestedPS,
		doExplode,
		doPairs,
		doAcrossFields,
	)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	*pargi = argi
	return transformer
}

// ----------------------------------------------------------------
type TransformerNest struct {
	fieldName string
	nestedFS  string
	nestedPS  string

	// For implode across fields
	regex *regexp.Regexp

	// For implode across records
	otherKeysToOtherValuesToBuckets *lib.OrderedMap

	recordTransformerFunc transforming.RecordTransformerFunc
}

// ----------------------------------------------------------------
func NewTransformerNest(
	fieldName string,
	nestedFS string,
	nestedPS string,
	doExplode bool,
	doPairs bool,
	doAcrossFields bool,
) (*TransformerNest, error) {

	this := &TransformerNest{
		fieldName: fieldName,
		nestedFS:  cliutil.SeparatorFromArg(nestedFS), // "pipe" -> "|", etc
		nestedPS:  cliutil.SeparatorFromArg(nestedPS),
	}

	// For implode across fields
	regexString := "^" + fieldName + "_[0-9]+$"
	regex, err := lib.CompileMillerRegex(regexString)
	if err != nil {
		fmt.Fprintf(
			os.Stderr,
			"%s %s: cannot compile regex [%s]\n",
			lib.MlrExeName(), verbNameNest, regexString,
		)
		os.Exit(1)
	}
	this.regex = regex

	// For implode across records
	this.otherKeysToOtherValuesToBuckets = lib.NewOrderedMap()

	if doExplode {
		if doPairs {
			if doAcrossFields {
				this.recordTransformerFunc = this.explodePairsAcrossFields
			} else {
				this.recordTransformerFunc = this.explodePairsAcrossRecords
			}
		} else {
			if doAcrossFields {
				this.recordTransformerFunc = this.explodeValuesAcrossFields
			} else {
				this.recordTransformerFunc = this.explodeValuesAcrossRecords
			}
		}
	} else {
		if doPairs {
			lib.InternalCodingErrorIf(true)
			// Should have been caught in CLI-parser.
		} else {
			if doAcrossFields {
				this.recordTransformerFunc = this.implodeValuesAcrossFields
			} else {
				this.recordTransformerFunc = this.implodeValueAcrossRecords
			}
		}
	}

	return this, nil
}

// ----------------------------------------------------------------
func (this *TransformerNest) Transform(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	this.recordTransformerFunc(inrecAndContext, outputChannel)
}

// ----------------------------------------------------------------
func (this *TransformerNest) explodeValuesAcrossFields(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	if !inrecAndContext.EndOfStream {

		inrec := inrecAndContext.Record
		originalEntry := inrec.GetEntry(this.fieldName)
		if originalEntry == nil {
			outputChannel <- inrecAndContext
			return
		}

		recordEntry := originalEntry
		mvalue := originalEntry.Value
		svalue := mvalue.String()

		// Not lib.SplitString so 'x=' will map to 'x_1=', rather than no field at all
		pieces := strings.Split(svalue, this.nestedFS)
		i := 1
		for _, piece := range pieces {
			key := this.fieldName + "_" + strconv.Itoa(i)
			value := types.MlrvalPointerFromString(piece)
			recordEntry = inrec.PutReferenceAfter(recordEntry, key, value)
			i++
		}

		inrec.Unlink(originalEntry)
		outputChannel <- inrecAndContext

	} else {
		outputChannel <- inrecAndContext // emit end-of-stream marker
	}
}

// ----------------------------------------------------------------
func (this *TransformerNest) explodeValuesAcrossRecords(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	if !inrecAndContext.EndOfStream {
		inrec := inrecAndContext.Record
		mvalue := inrec.Get(this.fieldName)
		if mvalue == nil {
			outputChannel <- inrecAndContext
			return
		}
		svalue := mvalue.String()

		// Not lib.SplitString so 'x=' will map to 'x=', rather than no field at all
		pieces := strings.Split(svalue, this.nestedFS)
		for _, piece := range pieces {
			outrec := inrec.Copy()
			outrec.PutReference(this.fieldName, types.MlrvalPointerFromString(piece))
			outputChannel <- types.NewRecordAndContext(outrec, &inrecAndContext.Context)
		}

	} else {
		outputChannel <- inrecAndContext // emit end-of-stream marker
	}
}

// ----------------------------------------------------------------
func (this *TransformerNest) explodePairsAcrossFields(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	if !inrecAndContext.EndOfStream {

		inrec := inrecAndContext.Record
		originalEntry := inrec.GetEntry(this.fieldName)
		if originalEntry == nil {
			outputChannel <- inrecAndContext
			return
		}

		mvalue := originalEntry.Value
		svalue := mvalue.String()

		recordEntry := originalEntry
		pieces := lib.SplitString(svalue, this.nestedFS)
		for _, piece := range pieces {
			pair := strings.SplitN(piece, this.nestedPS, 2)
			if len(pair) == 2 { // there is a pair
				recordEntry = inrec.PutReferenceAfter(
					recordEntry,
					pair[0],
					types.MlrvalPointerFromString(pair[1]),
				)
			} else { // there is not a pair
				recordEntry = inrec.PutReferenceAfter(
					recordEntry,
					this.fieldName,
					types.MlrvalPointerFromString(piece),
				)
			}
		}

		inrec.Unlink(originalEntry)
		outputChannel <- inrecAndContext

	} else {
		outputChannel <- inrecAndContext // emit end-of-stream marker
	}
}

// ----------------------------------------------------------------
func (this *TransformerNest) explodePairsAcrossRecords(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	if !inrecAndContext.EndOfStream {
		inrec := inrecAndContext.Record
		mvalue := inrec.Get(this.fieldName)
		if mvalue == nil {
			outputChannel <- inrecAndContext
			return
		}

		svalue := mvalue.String()
		pieces := lib.SplitString(svalue, this.nestedFS)
		for _, piece := range pieces {
			outrec := inrec.Copy()

			originalEntry := outrec.GetEntry(this.fieldName)

			// Put the new field where the old one was -- unless there's already a field with the new
			// name, in which case replace its value.
			pair := strings.SplitN(piece, this.nestedPS, 2)
			if len(pair) == 2 { // there is a pair
				outrec.PutReferenceAfter(originalEntry, pair[0], types.MlrvalPointerFromString(pair[1]))
			} else { // there is not a pair
				outrec.PutReferenceAfter(originalEntry, this.fieldName, types.MlrvalPointerFromString(piece))
			}

			outrec.Unlink(originalEntry)
			outputChannel <- types.NewRecordAndContext(outrec, &inrecAndContext.Context)
		}

	} else {
		outputChannel <- inrecAndContext // emit end-of-stream marker
	}
}

// ----------------------------------------------------------------
func (this *TransformerNest) implodeValuesAcrossFields(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	if !inrecAndContext.EndOfStream {
		inrec := inrecAndContext.Record

		var previousEntry *types.MlrmapEntry = nil
		fieldCount := 0
		var buffer bytes.Buffer
		for pe := inrec.Head; pe != nil; /* increment in loop */ {
			if this.regex.MatchString(pe.Key) {
				if fieldCount > 0 {
					buffer.WriteString(this.nestedFS)
				}
				buffer.WriteString(pe.Value.String())
				fieldCount++

				// Keep the location so we can implode in-place.
				if previousEntry == nil {
					previousEntry = pe.Prev
				}
				pnext := pe.Next
				inrec.Unlink(pe)
				pe = pnext
			} else {
				pe = pe.Next
			}
		}

		if fieldCount > 0 {
			newValue := types.MlrvalPointerFromString(buffer.String())
			if previousEntry == nil { // No record before the unlinked one, i.e. list-head.
				inrec.PrependReference(this.fieldName, newValue)
			} else {
				inrec.PutReferenceAfter(previousEntry, this.fieldName, newValue)
			}
		}

		outputChannel <- inrecAndContext

	} else {
		outputChannel <- inrecAndContext // emit end-of-stream marker
	}
}

// ----------------------------------------------------------------
func (this *TransformerNest) implodeValueAcrossRecords(
	inrecAndContext *types.RecordAndContext,
	outputChannel chan<- *types.RecordAndContext,
) {
	if !inrecAndContext.EndOfStream {
		inrec := inrecAndContext.Record

		originalEntry := inrec.GetEntry(this.fieldName)
		if originalEntry == nil {
			outputChannel <- inrecAndContext
			return
		}

		fieldValueCopy := originalEntry.Value.Copy()

		// Don't unset this.fieldName in the record, so we can implode in-place at the end.
		otherKeysJoined := inrec.GetKeysJoinedExcept(originalEntry)
		var otherValuesToBuckets *lib.OrderedMap = nil
		iOtherValuesToBuckets := this.otherKeysToOtherValuesToBuckets.Get(otherKeysJoined)
		if iOtherValuesToBuckets == nil {
			otherValuesToBuckets = lib.NewOrderedMap()
			this.otherKeysToOtherValuesToBuckets.Put(otherKeysJoined, otherValuesToBuckets)
		} else {
			otherValuesToBuckets = iOtherValuesToBuckets.(*lib.OrderedMap)
		}

		otherValuesJoined := inrec.GetValuesJoinedExcept(originalEntry)
		var bucket *tNestBucket = nil
		iBucket := otherValuesToBuckets.Get(otherValuesJoined)
		if iBucket == nil {
			bucket = newNestBucket(inrec)
			otherValuesToBuckets.Put(otherValuesJoined, bucket)
		} else {
			bucket = iBucket.(*tNestBucket)
		}

		pair := types.NewMlrmapAsRecord()
		pair.PutReference(this.fieldName, fieldValueCopy)
		bucket.pairs.PushBack(pair)

	} else { // end of input stream

		for pe := this.otherKeysToOtherValuesToBuckets.Head; pe != nil; pe = pe.Next {
			otherValuesToBuckets := pe.Value.(*lib.OrderedMap)
			for pf := otherValuesToBuckets.Head; pf != nil; pf = pf.Next {
				var buffer bytes.Buffer
				bucket := pf.Value.(*tNestBucket)
				outrec := bucket.representative
				bucket.representative = nil // ownership transfer

				i := 0
				for pg := bucket.pairs.Front(); pg != nil; pg = pg.Next() {
					pr := pg.Value.(*types.Mlrmap)
					if i > 0 {
						buffer.WriteString(this.nestedFS)
					}
					i++
					buffer.WriteString(pr.Head.Value.String())
				}

				// this.fieldName was already present so we'll overwrite it in-place here.
				outrec.PutReference(this.fieldName, types.MlrvalPointerFromString(buffer.String()))
				outputChannel <- types.NewRecordAndContext(outrec, &inrecAndContext.Context)
			}
		}

		outputChannel <- inrecAndContext // emit end-of-stream marker
	}
}

type tNestBucket struct {
	representative *types.Mlrmap
	pairs          *list.List
}

func newNestBucket(representative *types.Mlrmap) *tNestBucket {
	return &tNestBucket{
		representative: representative,
		pairs:          list.New(),
	}
}