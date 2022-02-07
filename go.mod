module github.com/johnkerl/miller

// The repo is 'miller' and the executable is 'mlr', going back many years and
// predating the Go port.
//
// If we had ./mlr.go then 'go build github.com/johnkerl/miller' then the
// executable would be 'miller' not 'mlr'.
//
// So we have cmd/mlr/main.go:
// * go build   github.com/johnkerl/miller/cmd/mlr
// * go install github.com/johnkerl/miller/cmd/mlr

// go get github.com/johnkerl/lumin@v1.0.0
// Local development:
// replace github.com/johnkerl/lumin => /Users/kerl/git/johnkerl/lumin

go 1.15

require (
	github.com/johnkerl/lumin v1.0.0
	github.com/kballard/go-shellquote v0.0.0-20180428030007-95032a82bc51
	github.com/lestrrat-go/strftime v1.0.5
	github.com/mattn/go-isatty v0.0.14
	github.com/pkg/errors v0.9.1 // indirect
	github.com/pkg/profile v1.6.0
	github.com/stretchr/testify v1.7.0
	golang.org/x/sys v0.0.0-20220204135822-1c1b9b1eba6a
	golang.org/x/term v0.0.0-20210927222741-03fcf44c2211
)
