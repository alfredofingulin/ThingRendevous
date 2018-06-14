#!python3

## @package trtgen.py
#
# Create a ThingRendevous trTi.c file 
# containing tasks, config, and resources
# by scanning source files for keywords and info.
#
#
# MIT License
# 
# Copyright (c) 2017 Alfred O Fingulin  FinguPublic@gmail.com
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

## Used for argument parsing.
#
import argparse

## Used to evaluate info next to a keyword; saves us parsing effort.
#
import ast

## Used to simplify file access.
#
import glob

## If we need glob, we need os.
#
import os

## Who doesn't use regular expressions?
#
import re

## If we need glob, we need sys.
#
import sys


## Parse command line arguments and make my_args available.
# 
def setup_args():
    sa_parser = argparse.ArgumentParser(description="Scan *.h *.c files; generate trTi.c task info file.\
        Details are described in a ThingRendevous tech note.")
    sa_parser.add_argument("-v", action="store_true", help="be verbose")
    sa_parser.add_argument("-t", action="store_true", help="generate report file [OUTDIR]/trtgenReport.out")
    sa_parser.add_argument("-indir", nargs="*", help="input directory list; defaults to .", default=["."])
    sa_parser.add_argument("-outdir", nargs="?", help="output directory for trTi.c; defaults to .", default=".")
    sa_parser.add_argument("-outfile", nargs="?", help="output filename; defaults to trTi.c", default="trTi.c")

    global my_args
    ## Filled with v (verbose) t (report), indir, outdir, and outfile info.
    #
    my_args = sa_parser.parse_args()


## Obvious; if my_args are verbose, send whatever x is to the console; else nothing.
#
def be_verbose(x):
    if (my_args.v):
        print(x)


## If my_args say open the report file, open write out invocation switches; else nothing.
#
# Creates, shares file handle my_rpt_f.
#
def rpt_f_open():
    if (my_args.t):

        global my_rpt_f
        ## "Private" global used by rpt_f_* functions.
        #
        my_rpt_f = open(my_args.outdir+"/trtgenReport.out", "w")

        my_rpt_f.write("ThingRendevous Task info Generator (trTi.c) for versions dd mmm yy\n")
        my_rpt_f.write("      -v: {}\n".format(my_args.v))
        my_rpt_f.write("      -t: {}, obviously.\n".format(my_args.t))
        my_rpt_f.write("  -indir: ")
        for idi in my_args.indir:
            my_rpt_f.write("{}  ".format(idi))
        my_rpt_f.write("\n")
        my_rpt_f.write(" -outdir: {}\n".format(my_args.outdir))
        my_rpt_f.write("-outfile: {}\n".format(my_args.outfile))

## If my_args say report file was opened, then close it; else nothing.
#
# Uses my_rpt_f.
#
def rpt_f_close():
    if (my_args.t):
        my_rpt_f.write("\nEnd trtgenReport.out \n")
        my_rpt_f.close()

## If my_args say report file was opened, write whatever x is to it; else nothing.
#
# Uses my_rpt_f.
#
def rpt_f_write(x):
    if (my_args.t):
        my_rpt_f.write(x)


## Rather than use lazy initialization, I'm gonna set up some globals here.
#
def setup_globals():
    
    global bailflag
    ## Set on unrecoverable processing problem.  Clean up is your problem.
    #
    bailflag = False
    
    global my_required_tasklist
    ## A list set up with the tasks we always expect in the system.
    #
    my_required_tasklist = ["Tti", "Tei"]
    
    global my_task_attribslist
    ## A list of attributes recognized; doubles as a key to the attribute.
    #
    my_task_attribslist = [ "someTask", "ID", "NominalPriority", "StackSize", "theTaskData", "StrmLen", "MsgLen", "MsgOrdering", "MsgPayload", "EvtLen" ]


## Puts together a list of files from my_args into my_filelist.
#
# Spews list; sets bailflag if nothing there.
#
def populate_filelist():
    fs = []
    for adir in my_args.indir:
        fs.append(adir+"/*.h")
        fs.append(adir+"/*.c")

    global my_filelist
    ## List of subdir + files to be searched.
    #
    my_filelist = []

    for afs in fs:
        for file in glob.glob(afs):
            my_filelist.append(file)

    if (my_filelist != []):
        be_verbose("Files to be searched:")
        be_verbose(my_filelist)
    else:
        nopopulationmsg = "No files to search.\n"+my_args.outfile+" not generated."
        be_verbose(nopopulationmsg)
        rpt_f_write(nopopulationmsg)
        bailflag = True


## Gather source lines from my_filelist into my_sourcelines_*
#
# Doesn't verbose, but does report.
#
def gather_sourcelines():
    rpt_f_write("Files searched and items found.  If it's recognizable, then it is here...\n")
    dt = re.compile("#define\s+trTask_\w+_\w+\s+\w+")
    vt = re.compile("void\s+trTask_\w+")

    global my_sourcelines_defines
    ## Contains all files' source lines using #define trTask_*
    #
    my_sourcelines_defines = []

    global my_sourcelines_voids
    ## Contains all files' source lines using void trTask_*
    #
    my_sourcelines_voids = []
    
    for somef in my_filelist:
        af = open(somef, "r")
        rpt_f_write(somef+":\n")
        for line in af:
            moji = dt.search(line)
            if (moji):
                my_sourcelines_defines.append(moji.group())
                rpt_f_write("\t"+moji.group()+"\n")
            else:
                moji = vt.search(line)
                if (moji):
                    my_sourcelines_voids.append(moji.group())
                    rpt_f_write("\t"+moji.group()+"\n")
        af.close()


## Go thru my_sourceline_voids, find tasks, and populate my_app_tasklist.
#
# Overlap with the my_required_tasklist is expected.
#
def populate_app_tasklist():
    vt = re.compile("void\s+trTask_")

    global my_app_tasklist
    ## A list set up with the tasks supplied by the application.  Get that from the source files.
    #
    my_app_tasklist = []

    for aline in my_sourcelines_voids:
        my_app_tasklist.append(vt.split(aline)[1])
    my_app_tasklist = list(set(my_app_tasklist))


## Put together the app and required subset tasks into one list.
#
# Assumes phoney Root has not been added.
# Purges dups.
# Is verbose but the report is obvious from the gathering source lines.
#
def generate_combined_tasklist():
    
    global my_combined_tasklist
    ## Really the reserved and app tasklists together.
    #
    my_combined_tasklist = my_required_tasklist + my_app_tasklist

    my_combined_tasklist = list(set(my_combined_tasklist))

    be_verbose("Tasks for processing:")
    be_verbose(my_combined_tasklist)


## Populate my_taskdict with all tasks and there default info.
#
# Uses my_combined_tasklist to do this.
# The phoney Root task will be added later.
# Overrides entries for Tti, Tei.
#
def populate_taskdict_defaults():
    
    global my_taskdict
    ## Here's where everything is stored.
    #
    # Task name is from my_app_tasklist or my_required_tasklist.
    # Every attribute key is from my_task_attribslist
    #
    my_taskdict = {}
    
    for at in my_combined_tasklist:
        my_taskdict[at] = { "leftLTE" : "End", "rightGT" : "End", "idx" : 0xFFFF, "someTask" : "trTask_"+at, "ID" : 0xFFFF, "NominalPriority" : 0xFFFF, "StackSize" : 0xFFFF, "theTaskData" : "NULL", "StrmLen" : 0, "MsgLen" : 0, "MsgOrdering" : 0, "MsgPayload" : 0, "EvtLen" : 0 }
    
    my_taskdict["Tei"] = { "leftLTE" : "End", "rightGT" : "End", "idx" : 0xFFFF, "someTask" : "trTask_Tei", "ID" : 64, "NominalPriority" : 64, "StackSize" : 1024, "theTaskData" : "NULL", "StrmLen" : 0, "MsgLen" : 0, "MsgOrdering" : 0, "MsgPayload" : 0, "EvtLen" : 0 }
    my_taskdict["Tti"] = { "leftLTE" : "End", "rightGT" : "End", "idx" : 0xFFFF, "someTask" : "trTask_Tti", "ID" : 128, "NominalPriority" : 128, "StackSize" : 1024, "theTaskData" : "NULL", "StrmLen" : 0, "MsgLen" : 0, "MsgOrdering" : 0, "MsgPayload" : 0, "EvtLen" : 0 }


## Use my_sourcelines_defines to fill-in attributes in my_taskdict.
#
def populate_taskdict_attribs():
    rpt_f_write("Items not understood; these will be ignored...\n")
    st=re.compile("#define\s+trTask_(\w+)_(\w+)\s+(\w+)")
    for aline in my_sourcelines_defines:
        sd = st.split(aline)
        if (sd[1] not in my_combined_tasklist):
            rpt_f_write(aline+"  # No such task.\n")
        elif (sd[2] not in my_task_attribslist):
            rpt_f_write(aline+"  # Item unknown.\n")
        elif ((sd[1] in my_required_tasklist) and (sd[2] != "StackSize")):
            rpt_f_write(aline+"  # Only trTask_"+sd[1]+"_StackSize allowed.\n")
        else:
            try:
                n = ast.literal_eval(sd[3])
            except:
                rpt_f_write(aline+"  # Value strange.\n")
            else:
                my_taskdict[sd[1]][sd[2]] = n


## Now that my_taskdict is populated, scan for errors and fix them.
#
# Terribly programmatic.
# bailflag set on unrecoverable ID problems.
#
def fixup_taskdict_attribs():

    rpt_f_write("Items modified, missing, or erronous...\n")

    for at in my_combined_tasklist:
        
        if (at in my_required_tasklist):  # for Tti/Tei, only check a subset.
            
            if (my_taskdict[at]["StackSize"] == 0xFFFF):
                my_taskdict[at]["StackSize"] = 1024
                rpt_f_write(my_taskdict[at]["someTask"]+"_StackSize not defined; forced to 1024; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_StackSize not defined; forced to 1024; continuing.")
                
            elif (my_taskdict[at]["StackSize"] < 8):
                my_taskdict[at]["StackSize"] = 8
                rpt_f_write(my_taskdict[at]["someTask"]+"_StackSize less than 8 bytes; forced to 8; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_StackSize less than 8; forced to 8; continuing.")

            sr = my_taskdict[at]["StackSize"] % 8
            
            if (sr > 0):
                my_taskdict[at]["StackSize"] = my_taskdict[at]["StackSize"] + (8-sr)
                rpt_f_write(my_taskdict[at]["someTask"]+"_StackSize not multiple of 8 bytes; increased it; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_StackSize not multiple of 8 bytes; increased it; continuing.")

        else:  # for app tasks, check everything.

            if (my_taskdict[at]["ID"] == 0xFFFF):
                rpt_f_write(my_taskdict[at]["someTask"]+"_ID not specified; must be specified; will bail.\n")
                print(my_taskdict[at]["someTask"]+"_ID not specified; must be specified; will bail.\n")
                bailflag = True
                
            elif ((my_taskdict[at]["ID"] <= 255) and (my_taskdict[at]["NominalPriority"] != my_taskdict[at]["ID"])):
                my_taskdict[at]["NominalPriority"] = my_taskdict[at]["ID"]
                rpt_f_write(my_taskdict[at]["someTask"]+"_ID is 1..255; NominalPriority forced to same value; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_ID  is 1..255; NominalPriority forced to same value; continuing.")
                
            elif ((my_taskdict[at]["NominalPriority"] <= 255) or (my_taskdict[at]["NominalPriority"] == 0xFFFF)):
                my_taskdict[at]["NominalPriority"] = 32767
                rpt_f_write(my_taskdict[at]["someTask"]+"_NominalPriority is not 256...0xFFFE or is not defined; forced to 32767; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_NominalPriority is not 256...0xFFFE or is not defined; forced to 32767; continuing.")
            
            if (my_taskdict[at]["StackSize"] == 0xFFFF):
                my_taskdict[at]["StackSize"] = 1024
                rpt_f_write(my_taskdict[at]["someTask"]+"_StackSize not defined; forced to 1024; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_StackSize not defined; forced to 1024; continuing.")
                
            elif (my_taskdict[at]["StackSize"] < 8):
                my_taskdict[at]["StackSize"] = 8
                rpt_f_write(my_taskdict[at]["someTask"]+"_StackSize less than 8 bytes; forced to 8; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_StackSize less than 8; forced to 8; continuing.")

            sr = my_taskdict[at]["StackSize"] % 8
            
            if (sr > 0):
                my_taskdict[at]["StackSize"] = my_taskdict[at]["StackSize"] + (8-sr)
                rpt_f_write(my_taskdict[at]["someTask"]+"_StackSize not multiple of 8 bytes; increased it; continuing.\n")
                be_verbose(my_taskdict[at]["someTask"]+"_StackSize not multiple of 8 bytes; increased it; continuing.")

            if (my_taskdict[at]["MsgLen"] > 0):
                
                if ((my_taskdict[at]["MsgOrdering"] != 0) and (my_taskdict[at]["MsgOrdering"] != 1)):
                    my_taskdict[at]["MsgOrdering"] = 0
                    rpt_f_write(my_taskdict[at]["someTask"]+"_MsgOrdering not FIFO (0) or Priority (1); forced to FIFO (0); continuing.\n")
                    be_verbose(my_taskdict[at]["someTask"]+"_MsgOrdering not FIFO (0) or Priority (1); forced to FIFO (0); continuing.")
                    
                if ((my_taskdict[at]["MsgPayload"] != 8) and (my_taskdict[at]["MsgPayload"] != 16) and (my_taskdict[at]["MsgPayload"] != 24) and (my_taskdict[at]["MsgPayload"] != 32)):
                    my_taskdict[at]["MsgPayload"] = 8
                    rpt_f_write(my_taskdict[at]["someTask"]+"_MsgPayload not 8, 16, 24, or 32; forced to 8 bytes; continuing.\n")
                    be_verbose(my_taskdict[at]["someTask"]+"_MsgPayload not 8, 16, 24, or 32; forced to 8 bytes; continuing.")
                    
            else:
                my_taskdict[at]["MsgOrdering"] = 0
                my_taskdict[at]["MsgPayload"] = 0


## Assumes Root has not been added to my_combined_tasklist, gets the size, sets up constants the firmware will need.
#
def generate_NumOfTasks():
    
    global my_trTi_NumOfTasks
    ## All the tasks, without phoney Root.
    #
    my_trTi_NumOfTasks = len(my_combined_tasklist)

    global my_trTi_NumOfTasks_PlusOne
    ## All the tasks, including phoney Root.
    #
    my_trTi_NumOfTasks_PlusOne = my_trTi_NumOfTasks + 1


## Here come the strings for number of tasks.  As long as we're here and we've done it above...
#
def build_NumOfTasks():
    return "const uint16_t trTi_NumOfTasks = "+str(my_trTi_NumOfTasks)+";\n"\
            + "const uint16_t trTi_NumOfTasks_PlusOne = "+str(my_trTi_NumOfTasks_PlusOne)+";\n"


## Here's the header and the include files.
#
def build_trTic_preamble():
    return "/** \\file\n"\
            +"*   \\brief ThingRendevous's Task Info implementation \n"\
            +"*\n"\
            +"* Generated by Trtgen.py.\n"\
            +"*/\n"\
            +"#include <stdint.h>\n"\
            +"#include <stdbool.h>\n"\
            +"#include <stddef.h>\n"\
            +"#include \"trBidt.h\"\n"\
            +"#include \"trPl.h\"\n"\
            +"#include \"trEvt.h\"\n"\
            +"#include \"trMsg.h\"\n"\
            +"#include \"trStrm.h\"\n"\
            +"#include \"trT.h\"\n"\
            +"#include \"trEvtP.h\"\n"\
            +"#include \"trMsgP.h\"\n"\
            +"#include \"trStrmP.h\"\n"\
            +"#include \"trTP.h\"\n"\
            +"#include \"trTi.h\"\n"


## String for the priority element list, sized for the number of tasks.
#
def build_TaskPriEle():
    return "struct trPl_Lst_s trTi_TaskReadyResrc_pe["+str(my_trTi_NumOfTasks)+"];  /* This is sized trTi_NumOfTasks. */\n" \
            + "struct trPl_Lst_s trTi_TaskTimeout_pe["+str(my_trTi_NumOfTasks)+"];  /* This is sized trTi_NumOfTasks. */\n"


## Uses a task key to look up an attribute key, builds priority list's basic name for same.
#
def build_basic_PL_name(tk, ak):
    ret_val = ""
    if (ak == "MsgLen"):
        ret_val = "trTi_Msg_PL_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "EvtLen"):
        ret_val = "trTi_Evt_PL_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    return ret_val


## Uses a task key to look up an attribute key, builds payload's basic name for same.
#
def build_basic_PY_name(tk, ak):
    ret_val = ""
    if (ak == "MsgLen"):
        ret_val = "trTi_Msg_PY_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    return ret_val


## Uses a task key to look up an attribute key, builds data block's basic name for same.
#
def build_basic_DB_name(tk, ak):
    ret_val = ""
    if (ak == "StrmLen"):
        ret_val = "trTi_Strm_DB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "MsgLen"):
        ret_val = "trTi_Msg_DB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "EvtLen"):
        ret_val = "trTi_Evt_DB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "StackSize"):
        ret_val = "trTi_Task_SB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    return ret_val


## Uses a task key to look up an attribute key, examines it, builds priority lists or comments for same.
#
def build_PL(tk, ak):
    ret_val = ""
    if (ak == "MsgLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_PL_name(tk,ak)+". */\n"
        elif ( my_taskdict[tk]["MsgOrdering"] == 0 ):
            ret_val = "/* Did not build "+build_basic_PL_name(tk,ak)+" because FIFO ordering; ci_pl_p will be NULL. */\n"
        else:
            ret_val = "struct trPl_Lst_s "+build_basic_PL_name(tk,ak)+"["+str(my_taskdict[tk][ak])+"];\n"
    if (ak == "EvtLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_PL_name(tk,ak)+". */\n"
        else:
            ret_val = "struct trPl_Lst_s "+build_basic_PL_name(tk,ak)+"["+str(my_taskdict[tk][ak])+"];\n"
    return ret_val


## Uses a task key to look up an attribute key, examines it, builds payload blocks or comments for same.
#
def build_PY(tk, ak):
    ret_val = ""
    if (ak == "MsgLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_PY_name(tk,ak)+". */\n"
        else:
            ret_val = "uint64_t "+build_basic_PY_name(tk,ak)+"["+str(int((my_taskdict[tk]["MsgLen"]*my_taskdict[tk]["MsgPayload"])/8))+"]; /* uint64_t forces each 8 byte element on an 8 byte boundary */\n"
    return ret_val


## Uses a task key to look up an attribute key, examines it, builds data blocks or comments for same.
#
def build_DB(tk, ak):
    ret_val = ""
    if (ak == "StrmLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_DB_name(tk,ak)+". */\n"
        else:
            ret_val = "uint8_t "+build_basic_DB_name(tk,ak)+"["+str(my_taskdict[tk][ak])+"];\n"
    elif (ak == "MsgLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_DB_name(tk,ak)+". */\n"
        else:
            ret_val = "struct trMsgP_DB_s "+build_basic_DB_name(tk,ak)+"["+str(my_taskdict[tk][ak])+"];\n"
    elif (ak == "EvtLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_DB_name(tk,ak)+". */\n"
        else:
            ret_val = "struct trEvtP_DB_s "+build_basic_DB_name(tk,ak)+"["+str(my_taskdict[tk][ak])+"];\n"
    elif (ak == "StackSize"):
        ret_val = "uint64_t "+build_basic_DB_name(tk,ak)+"["+str(int(my_taskdict[tk][ak]/8))+"]; /* uint64_t forces each 8 byte element on an 8 byte boundary */\n"
    return ret_val


## Uses a task key to look up an attribute key, builds control block's basic name for same.
#
def build_basic_CB_name(tk, ak):
    ret_val = ""
    if (ak == "StrmLen"):
        ret_val = "trTi_Strm_CB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "MsgLen"):
        ret_val = "trTi_Msg_CB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "EvtLen"):
        ret_val = "trTi_Evt_CB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "StackSize"):
        ret_val = "trTi_Task_CB_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    return ret_val

## Uses a task key to look up an attribute key, examines it, builds control blocks or comments for same.
#
def build_CB(tk, ak):
    ret_val = ""
    if (ak == "StrmLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_CB_name(tk,ak)+". */\n"
        else:
            ret_val = "struct trStrmP_CB_s "+build_basic_CB_name(tk,ak)+";\n"
    elif (ak == "MsgLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_CB_name(tk,ak)+". */\n"
        else:
            ret_val = "struct trMsgP_CB_s "+build_basic_CB_name(tk,ak)+";\n"
    elif (ak == "EvtLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_CB_name(tk,ak)+". */\n"
        else:
            ret_val = "struct trEvtP_CB_s "+build_basic_CB_name(tk,ak)+";\n"
    elif (ak == "StackSize"):
        if (my_taskdict[tk][ak] < 8):
            ret_val = "/* StackSize less than eight!?  SO WRONG IT'S STUPID!!! */\n"
        else:
            ret_val = "struct trTP_CB_s "+build_basic_CB_name(tk,ak)+";\n"
    return ret_val

## Uses a task key to look up an attribute key, builds config info's basic name for same.
#
def build_basic_CI_name(tk, ak):
    ret_val = ""
    if (ak == "StrmLen"):
        ret_val = "trTi_Strm_CI_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "MsgLen"):
        ret_val = "trTi_Msg_CI_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "EvtLen"):
        ret_val = "trTi_Evt_CI_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    elif (ak == "StackSize"):
        ret_val = "trTi_Task_CI_"+str(tk)+"_"+str(my_taskdict[tk]["ID"])+"_"+str(my_taskdict[tk]["idx"])
    return ret_val

## Uses a task key to look up an attribute key, examines it, builds config info or comments for same.
#
def build_CI(tk, ak):
    ret_val = ""
    if (ak == "StrmLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_CI_name(tk,ak)+". */\n"
        else:
            ret_val = "const struct trStrmP_CI_s "+build_basic_CI_name(tk,ak)+" =\n"\
                +"{\n"\
                +"\t"+str(my_taskdict[tk][ak])+", /* ci_StrmLen */ \n"\
                +"\t&"+build_basic_CB_name(tk,ak)+", /* ci_cb_p */ \n"\
                +"\t&"+build_basic_DB_name(tk,ak)+"[0]  /* ci_db_p */ \n"\
                +"};\n"
    elif (ak == "MsgLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_CI_name(tk,ak)+". */\n"
        else:
            ret_val = "const struct trMsgP_CI_s "+build_basic_CI_name(tk,ak)+" =\n"\
                +"{\n"\
                +"\t"+str(my_taskdict[tk][ak])+", /* ci_MsgLen */ \n"\
                +"\t"+str(my_taskdict[tk]["MsgOrdering"])+", /* ci_MsgOrdering */ \n"\
                +"\t"+str(my_taskdict[tk]["MsgPayload"])+", /* ci_MsgPayload */ \n"\
                +"\t&"+build_basic_CB_name(tk,ak)+", /* ci_cb_p */ \n"\
                +"\t&"+build_basic_DB_name(tk,ak)+"[0],  /* ci_db_p */ \n"\
                +"\t(uint8_t *) &"+build_basic_PY_name(tk,ak)+"[0], /* ci_py_p */ \n"
            if (my_taskdict[tk]["MsgOrdering"] == 0):
                ret_val = ret_val+"\tNULL /* ci_pl_p */ \n"
            else:
                ret_val = ret_val + "\t&"+build_basic_PL_name(tk,ak)+"[0] /* ci_pl_p */ \n"
            ret_val = ret_val +"};\n"
    elif (ak == "EvtLen"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* No need for "+build_basic_CI_name(tk,ak)+". */\n"
        else:
            ret_val = "const struct trEvtP_CI_s "+build_basic_CI_name(tk,ak)+" =\n"\
                +"{\n"\
                +"\t"+str(my_taskdict[tk][ak])+", /* ci_EvtLen */ \n"\
                +"\t&"+build_basic_CB_name(tk,ak)+", /* ci_cb_p */ \n"\
                +"\t&"+build_basic_DB_name(tk,ak)+"[0],  /* ci_db_p */ \n"\
                +"\t&"+build_basic_PL_name(tk,ak)+"[0]  /* ci_pl_p */ \n"\
                +"};\n"
    elif (ak == "StackSize"):
        if (my_taskdict[tk][ak] == 0):
            ret_val = "/* StackSize Zero!?  SO WRONG IT'S STUPID!!! */\n"
        else:
            ret_val = "const struct trTP_CI_s "+build_basic_CI_name(tk,ak)+" =\n"\
                    +"{\n"\
                    +"\t&"+build_basic_CB_name(tk,ak)+", /* ci_cb_p */ \n"\
                    +"\t"+my_taskdict[tk]["someTask"]+", /* ci_someTask */ \n"
            if (my_taskdict[tk]["theTaskData"] == "NULL"):
                ret_val = ret_val+"\tNULL, /* ci_theTaskData_p */ \n"
            else:
                ret_val = ret_val+"\t&"+my_taskdict[tk]["theTaskData"]+", /* ci_theTaskData_p */ \n"
            ret_val = ret_val +"\t(uint8_t *) &"+build_basic_DB_name(tk,ak)+"[0], /* ci_sb_p */ \n"\
                    +"\t"+str(my_taskdict[tk]["StackSize"])+", /* ci_StackSize */ \n"\
                    +"\t"+str(my_taskdict[tk]["NominalPriority"])+" /* ci_NominalPriority */ \n"\
                    +"};\n"

    return ret_val


## So we don't have to include things, build task prototype.  
#
def built_taskprotos(tk):
    return "void "+my_taskdict[tk]["someTask"]+"(void *theTaskData);\n"

## Constant structure opening for the Task Info data.
#
def build_TaskInfoList_start():
    return "const struct trTi_T_IB_s trTi_TaskInfoList["+str(my_trTi_NumOfTasks)+"] = /* This is sized trTi_NumOfTasks. */\n{\n"

## Closing brace for Task Info data.
#
def build_TaskInfoList_end():
    return "};\n"

## Builds Task Info data entry for a task key in my_combined_tasklist.  Root can be in the mix, but shouldn't be used.
#
#{
#    x,  /* ib_t_p */
#    x,  /* ib_evt_p */
#    x,  /* ib_msg_p */
#    x   /* ib_strm_p */
#}
#
def build_TaskInfoList_content(tk, needcomma=True):
    ret_val = "\t{\n"\
            + "\t\t&"+build_basic_CI_name(tk,"StackSize")+", /* ib_t_p */\n"

    if (my_taskdict[tk]["EvtLen"] == 0):
        ret_val = ret_val + "\t\tNULL, /* ib_evt_p "+tk+" has no Evt. */\n"
    else:
        ret_val = ret_val + "\t\t&"+build_basic_CI_name(tk,"EvtLen")+ ", /* ib_evt_p */\n"

    if (my_taskdict[tk]["MsgLen"] == 0):
        ret_val = ret_val + "\t\tNULL, /* ib_msg_p "+tk+" has no Msg. */\n"
    else:
        ret_val = ret_val + "\t\t&"+build_basic_CI_name(tk,"MsgLen")+ ", /* ib_msg_p */\n"

    if (my_taskdict[tk]["StrmLen"] == 0):
        ret_val = ret_val + "\t\tNULL /* ib_strm_p "+tk+" has no Strm. */\n"
    else:
        ret_val = ret_val + "\t\t&"+build_basic_CI_name(tk,"StrmLen")+ " /* ib_strm_p */\n"

    ret_val = ret_val + "\t}"
    if (needcomma):
        ret_val = ret_val+",\n"
    else:
        ret_val = ret_val+"\n"
    return ret_val


## Iterate over all tasks but Root; spew constant content.  Assumes idx has been set up, and Root is idx NumOfTasks (that's how Root is avoided).
#
def build_TaskInfoList_allcontent():
    ret_val = ""
    i = 0
    while (i < my_trTi_NumOfTasks):
        for a in my_combined_tasklist:
            if (my_taskdict[a]["idx"] == i):
                if (i < (my_trTi_NumOfTasks - 1)):
                    ret_val = ret_val + build_TaskInfoList_content(a)
                else:
                    ret_val = ret_val + build_TaskInfoList_content(a, False)
                break
        i = i + 1
    return ret_val


## Opening constant structure for the ID tree
#
def build_TaskIDTree_start():
    return "const struct trBidt_Node_const_s trTi_TaskIDTree["+str(my_trTi_NumOfTasks_PlusOne)+"] = /* This is sized trTi_NumOfTasks_PlusOne. */\n{\n"


## That final brace for the ID tree.
#
def build_TaskIDTree_end():
    return "};\n"


## Strings for ID tree content.
#
#{
#    x, /* the_ID for key idx */
#    x, /* child_left is key */  
#    x  /* child_right is key */ 
#}
#
def build_TaskIDTree_content(tk, needcomma=True):
    ret_val = "\t{\n"\
            + "\t\t"+str(my_taskdict[tk]["ID"])+", /* the_ID for "+tk+" with idx "+str(my_taskdict[tk]["idx"])+" */\n"
    if (my_taskdict[tk]["leftLTE"] == "End"):
        ret_val = ret_val \
            + "\t\t"+str(0xFFFF)+", /* child_left is End */\n"
    else:
        ret_val = ret_val \
            + "\t\t"+str(my_taskdict[my_taskdict[tk]["leftLTE"]]["idx"])+", /* child_left is "+my_taskdict[tk]["leftLTE"]+" at this idx. */\n"
    if (my_taskdict[tk]["rightGT"] == "End"):
        ret_val = ret_val \
                + "\t\t"+str(0xFFFF)+" /* child_right is End */\n"
    else:
        ret_val = ret_val \
                + "\t\t"+str(my_taskdict[my_taskdict[tk]["rightGT"]]["idx"])+"  /* child_right is "+my_taskdict[tk]["rightGT"]+" at this idx. */\n"
    ret_val = ret_val \
            + "\t}"
    if (needcomma):
        ret_val = ret_val+",\n"
    else:
        ret_val = ret_val+"\n"
    return ret_val


## Iterate over my_combined_tasklist and spew ID content.  Assumes Root is in the mix, now.
#
def build_TaskIDTree_allcontent():
    ret_val = ""
    i = 0
    while (i < my_trTi_NumOfTasks):  # Note it's not _PlusOne.
        for a in my_combined_tasklist:
            if (my_taskdict[a]["idx"] == i):
                ret_val = ret_val + build_TaskIDTree_content(a)
                break
        i = i + 1
    ret_val = ret_val + build_TaskIDTree_content("Root", False)  # And here's _PlusOne!
    return ret_val


## Find an unassigned task in my_taskdict.
#
# A task with index = 0xFFFF is considered unassigned.
#
def find_unassignedtask_btree():
    ret_val = "End"
    for at in my_taskdict:
        if (my_taskdict[at]["idx"] == 0xFFFF):
            ret_val = at
            break
    return ret_val


## Set up my_taskdict's task idx's for btree stringing.
#
# Start by adding Root, making it idx my_trTi_NumOfTasks
# (remember: the TaskIDTree includes Root at the end and is _PlusOne length!)
# forcing tree positions for Tti and Tei.
# Pick any app task and make it the "right-side-Root".
# Name that task in my_taskapp_rootk, make it idx 2.
# All subseqent enteries are idx 3...my_trTi_NumOfTasks-1.
#
def setup_task_btree():
    my_taskdict["Root"] =  { "leftLTE" : "Tei", "rightGT" : "End", "idx" : my_trTi_NumOfTasks, "someTask" : "trTask_Root", "ID" : 0, "NominalPriority" : 0, "StackSize" : 8, "theTaskData" : "NULL", "StrmLen" : 0, "MsgLen" : 0, "MsgOrdering" : 0, "MsgPayload" : 0, "EvtLen" : 0 }
    my_taskdict["Tei"]["idx"] = 0
    my_taskdict["Tei"]["rightGT"] = "Tti"
    my_taskdict["Tti"]["idx"] = 1

    global my_taskapp_rootk
    ## The task ID tree root is one thing, but the application side is another; this'll help short-circuit the search.
    #
    my_taskapp_rootk = find_unassignedtask_btree()
    
    my_taskdict[my_taskapp_rootk]["idx"] = 2
    my_taskdict["Root"]["rightGT"] = my_taskapp_rootk


## At the given task, insert the unassigned task and set the idx value; can't insert/append? then next one and try again.
#
def insertappend_task_btree(attaskk, unassk, i):
    if (my_taskdict[unassk]["ID"] > my_taskdict[attaskk]["ID"]):
        if (my_taskdict[attaskk]["rightGT"] == "End"):
            my_taskdict[attaskk]["rightGT"] = unassk
            my_taskdict[unassk]["idx"] = i
        else:
            insertappend_task_btree(my_taskdict[attaskk]["rightGT"], unassk, i)
    else:
        if (my_taskdict[attaskk]["leftLTE"] == "End"):
            my_taskdict[attaskk]["leftLTE"] = unassk
            my_taskdict[unassk]["idx"] = i
        else:
            insertappend_task_btree(my_taskdict[attaskk]["leftLTE"], unassk, i)


## Go thru my_taskdict, string together a tree by ID value.
#
def generate_task_btree():
    i = 3
    unass = find_unassignedtask_btree()
    while (unass != "End"):
        insertappend_task_btree(my_taskapp_rootk, unass, i)
        i = i + 1
        unass = find_unassignedtask_btree()


## Where the task indexes get built.
#
def anything_we_forgot_goddammit():
    ret_val = "const uint16_t trTi_Tti_ID = "+str(my_taskdict["Tti"]["ID"])+";\n" \
        + "const uint16_t trTi_Tti_idx = "+str(my_taskdict["Tti"]["idx"])+";\n" \
        + "const uint16_t trTi_Tei_ID = "+str(my_taskdict["Tei"]["ID"])+";\n" \
        + "const uint16_t trTi_Tei_idx = "+str(my_taskdict["Tei"]["idx"])+";\n"
    return ret_val


## Here's where the output file is made.
#
def emit_trTic():

    trTic_f = open(my_args.outdir+"/"+my_args.outfile, "w")
    
    trTic_f.write(build_trTic_preamble())

    for tk in my_combined_tasklist:
        for ak in ["StrmLen", "MsgLen", "EvtLen"]:
            trTic_f.write(build_PL(tk,ak))
            trTic_f.write(build_PY(tk,ak))
            trTic_f.write(build_DB(tk,ak))
            trTic_f.write(build_CB(tk,ak))
            trTic_f.write(build_CI(tk,ak))
        trTic_f.write(build_DB(tk, "StackSize"))
        trTic_f.write(build_CB(tk, "StackSize"))
        trTic_f.write(built_taskprotos(tk))
        trTic_f.write(build_CI(tk, "StackSize"))

    trTic_f.write(build_NumOfTasks())

    trTic_f.write(build_TaskInfoList_start())
    trTic_f.write(build_TaskInfoList_allcontent())
    trTic_f.write(build_TaskInfoList_end())
    
    trTic_f.write(build_TaskIDTree_start())
    trTic_f.write(build_TaskIDTree_allcontent())
    trTic_f.write(build_TaskIDTree_end())

    trTic_f.write(build_TaskPriEle())
    
    trTic_f.write(anything_we_forgot_goddammit())
    
    trTic_f.close()


# And away we go.

print("ThingRendevous Task info Generator (trTi.c) for versions dd mmm yy")

setup_args()
rpt_f_open()

setup_globals()
populate_filelist()

if bailflag:
    rpt_f_close()
    exit()

gather_sourcelines()

populate_app_tasklist()
generate_combined_tasklist()

populate_taskdict_defaults()

populate_taskdict_attribs()
fixup_taskdict_attribs()

if bailflag:
    rpt_f_close()
    exit()

generate_NumOfTasks()

setup_task_btree()
generate_task_btree()

if not bailflag:
    
    be_verbose("Writing output file.")
    emit_trTic()
    rpt_f_write("Generated "+my_args.outdir+"/"+my_args.outfile+".")

rpt_f_close()
exit()
