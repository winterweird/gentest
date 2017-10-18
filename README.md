# Generate simple test data of different formats

I put this up on GitHub because I used it to generate random test data for an
assignment I had for uni, and I decided to make the generating part transparent.
I actually wrote it some time ago in order to generate simple test data for
[the Kattis judge system](https://open.kattis.com), but it ended up being useful here
too.

Note that this program also relies on some OTHER header-only library I wrote
_even longer_ ago which is intended to make parsing command line arguments
simpler. The original version of this program just used the header as per
installed on my system, but I've included it locally in this project to avoid
any external references.

## But how does it work?

More or less like this:

```
$ make
$ bin/gentest
No sample size argument provided.
Usage: gentest [OPTIONS]

Options:
  -n <sample-size>   Specify number of samples (required)
  --number-samples

  -e <limit>         Specify limit (of int size, length, etc.) - default 1000
  --limit-end

  -b <limit>         Specify lowest value (of int size, length, etc) - default 1
  --limit-begin

  -o <file>          Specify output file
  --output-file

  -i                 Generate integers
  --integer

  -d                 Generate doubles
  --double

  -s                 Generate strings
  --string

  -t <string-type>   Specify type of characters in string - default alpha
  --char-type

  -v                Print more information about generated data
  --verbose
```

For the record, the `-n <sample-size>` option is mandatory. Why didn't I make it
the first or the last argument or something and dropped the `-n` part? Limitation
in the command line argument parser program I never got around to fixing.
Everything else has reasonable defaults, I think, but if you're not sure, just
specify it.

The command I used to generate the sample input used in the assignment, for the
record, was:

```
$ gentest -n N -b 1 -e 1000 -o FILENAME -i -v
```

... where `N` was the number of "file sizes" and `FILENAME` was the name of the
generated input file. Note that the `-b` and `-e` options are technically
superfluous, but I like being explicit.

Disclaimer: I haven't actually tested this on any system but mine, so I can't
guarantee it works. Also, I haven't really tested double generation either, I
just brainfarted a method of generating them that made sense to me at the time.
Don't judge me plz.
