# ECE595-PA4
Zero-skew clock tree synthesis with inverter/repeater insertion

## Compilation
```
make pa4
```

## Usage
```
./pa4 input_filename1 input_filename2 input_filename3 output_filename1 output_filename2
```
The file specified by **input_filename1** contains the parameters of inv0. This file should contain a
single line printed with the format "%.10le %.10le %.10le\n", where the first double is the
input capacitance (in F), the second double is the output capacitance (in F), and the third double
is the output resistance (in Ω).

The file specified by **input_filename2** contains the wire parameters. This file should contain a single
line printed with the format "%.10le %.10le\n" where the first double is the per-unit-length
wire resistance (in Ω/unit length) and the second double is the per-unit-length wire capacitance
(in F/unit length).

The file specified by **input_filename3** contains information for the sink nodes. Each line in the file
is printed with the format "%d(%.10le %.10le %.10le)\n", where the int is the label of the
sink node, the first double is the sink capacitance (in F), and the next two double’s are the x- and
y-coordinates.

The executable prints the topology information to a text file whose filename is given as **output_filename1**.

The executable also writes the topology information to a binary file whose filename is given as **output_filename2**.
