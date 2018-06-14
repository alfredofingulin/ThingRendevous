# ThingRendevous; A Real-Time Multi-tasking Pre-emptive Kernel for Embedded Systems

Kind of says it all, doesn't it?  
See Motivation and Technical Overview for unique features.

Targeted to ARM Cortex M4 uPs; ports are planned.

Developed using ARM's GCC toolset in a Linux environment via Eclipse
(you don't have to pay for anything), 
but is tool independant (should work with IAR or almost anybody's tools).

# Motivation

For many embedded systems, I've found:

* Linux is too much
* RTOS' with the usual semaphores, queues, and priority mechanisms mean re-inventing the wheel, again and again.

What I wanted and delivered:

* A pre-emptable kernel supporting pre-emptable tasks
* Tasks can run synchronously or asynchronously (block or march-ahead)
* Rendevous between tasks using
    * prioritized events
    * block oriented messages, either FIFO or prioritized
    * byte steams, always FIFO.

# An Example

The test program in test/STM32F429Nucleo144/trTst07 is a good example.

* Three tasks: Jonny, Benton, and Race; and one pushbutton interrupt handler.
* Benton and Race
    * have equal priority
    * ping-pong an event with each other
    * when the task has the event, it does work
    * upon work completion sends the event to the other task
* Meanwhile: there's a pushbutton interrupt
    * pushing the button sends an event to Jonny
* Jonny is a high priority task
    * he can interrupt whoever is running, Benton or Race
    * Jonny panics
    * then Benton and Race get back to work.
  
Anything prefaced by the identifiers

* trAPI is a ThingRendevous call
* trTask is a kernel configuration item.

# Where To Find Things

Everything is here, but you might not want it all.

## inc/ and src/ Just the Source Code (and a tool you need in tool/)

* inc/ contains the API header files.  It's all your app needs.
* src/ contains ThingRendevous itself.  Everything.  The code follows: 
    * Linux formatting style
    * GCC convention for in-line assember (and doesn't use ARM's CMSIS)
    * Doxygen for all documentation (we're old-school).
* But ThingRendevous needs a configuration data structure.  The data structure is built from keywords in your source code.
    * tool/trtgen.py is that tool; requires python3
    * keywords and usage described in doc/TN09_AllAboutTrTasks.txt
    
All code is Doxygen-ized.

## doc/ All the Docs (and a word about Doxygen)

All the docs are .txt files.

* Spec files contain a requirements Specifiction and description
* TN files are Technical Notes 
    * intended for app writers
    * it's mostly tool acquisition and configuration and "How do I?..."
        * ...build a static ThingRendevous archive
        * ...set-up Eclipse
        * ...find ARM's GCC compiler
        * and other other stuff
* uDN are Design Notes
    * if you want to look under-the-hood, here's the hood...
    * further subdivided by uP and board.

Each .txt file is a Doxygen "source" file.

Try using the command line "doxygen Doxyfile" at the root.
Watch the stuff come out.
Point your browswer to html/index.html.
Isn't that nice?

## ext/ uP and Board Support Code

ext/ contains all uP- and board-specific code.
Each uP and board has it's own subdirectory.
Each subdir contains 

* a re-distribution of vendor-supplied code, tweaked in ways I've found helpful
* custom code I've developed, usually start-up code and such.

## test/ Test Code

Each subdirectory contains test code peculiar 
to a uP or an evaluation board.

## tool/ Scripts and Such

I found 'em helpful.  You may too.

# License

MIT, folks.  Fell free to modify and incorp into your money-maker.
But gimme some credit, dammit; somewhere, anywhere.

# Technical Overview

ThingRendevous, aka Tr, 
is a real-time multitasking priority-based scheduling kernel 
for embedded systems.

The kernel follows Hoare's Communicating Sequential Processes pattern: 

* The task is the fundamental computational unit.
* Each task gets its own thread (that is: each task has it's own stack).
    * Need multiple threads?  Use the same task code with a different stack.
* Tasks communication using:
    * Events 
        * brief 32-bit chunks
        * ordered priority of the sending task 
        * identified by the sending task
    * Messages 
        * blocks of data
        * ordered by priority or FIFO (your choice)
        * identified by the sending task
    * Streams
        * of bytes
        * always FIFO
        * always anonymous.
  
It's very similar to Ada's task type,
but entry-s/accept-s are explicitly events/messages/streams.

* Every task can have none or one, and any combo of:
    * event queue
    * message queue
    * stream
* Only the owning task can read from (aka accept) its events, messages, or streams.
* Every task can write to (aka enter) every other task's event, message, or stream.
* Both pre-empting and non-pre-empting calls may be used (see the Spec for details).
* All non-pre-empting calls may be made from interrupts.
* Interrupts are nest-able and re-enable-able.

To build data structures need by Tr, a seperate tool is used.

* tool/trtgen.py is that tool; requires python3
* scans your code for keywords
* keywords and usage described in doc/TN09_AllAboutTrTasks.txt
* builds a file, trTi.c, that's part of your app.

Tr uses this data structure to operate on APIs.

Tr always operates as its own ARM process, with its own stack.
Tr is interruptable.  Tr uses 

* the usual b-trees to resolve IDs
* priority-lists to figure out who should run
* tracks task states
* and worries context saves/restores.

Pretty pedestrian stuff.

Tr does not use the usual interrupts-off critical section meme.

Tr's uses a private section; it's just an SVC exception.
Tr takes advantage of SVC's high priority 
to perform crucial operations, even during interrupts.

* This avoids ARM exception and interrupt priority issues (you are responsible for prioritizing interrupts).
* It gets around ARM process priviledge conumndrums (tho everything is priviledged).
* Treats ARM handlers and threads transparently.

# Technical Limitations

ThingRendevous does not support run-time (dynamic) task creation.
All tasks and resource requirements are static
and constructed at build-time.
When you build a system, 
all the tasks present at build-time are all the tasks you get.

Embedded systems I've encountered  
rarely perform dynamic task creation.
You build it; you flash it; you got it.

If you need dynamic process creation, 
well, that makes the case for embedding Linux, doesn't it?

Tr's private section scheme via SVC means
a lot of register stacking and un-stacking.
This may be less of a problem than we think.

Finally: This is a message-passing kernel.
Copy-time can be an issue 
(So why not just toss pointers around by exchanging message blocks?
See?  You're gonna do it anyway).

# Who's Responsible For This Thing?

Alfred O Fingulin  FinguPublic@gmail.com  alfredofingulin

It's his thing, and his thing alone.

# How Do I Stop Him?

You can't.  Only The Kielbasa Kid can stop him.

-30-
