# Tape to Printer
**Tape to printer** it's simple program which make use of two devices of umps2, the tape and the printer.
To create all the source file needed in order to run the machine, just run
```
make
```
in the main folder.

The program reads the content of various tapes and send it on a printer.
To show an example of the operation, we will use a famous song for demonstration purposes.

There are two main functions, the first one *tirulero_maker_1* uses three tapes at the same time, jumping from one to the others mixing the content according to specific instructions and printing it on the first printer. The final result it's the italian version of the famous song from Disney's Robin Hood movie.
The second function, *tirulero_maker_2*, uses only one tape and build the song by jumping from block to blocks, rewinding the tape and skipping blocks already printed. The result is the same song but printed on the second printer.

We decided to use this song because it has two repetition (the names, *Robin Hood e Little John*, and the refrain) and it's well impressed in our mind. It's a perfect sample for our example.