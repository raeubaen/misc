#!/bin/tcsh
#
# ------------------------------------------------------------------------
# Get arguments
# ------------------------------------------------------------------------
#

if( $#argv != 7 ) then
  echo "USAGE >>> doroot.csh Holder1 Holder2 Holder1_Holder2 Nfiles [1-4] [Nstep] version"
  exit
endif
echo "starting the code"
set Holder1 = $1
set Holder2 = $2
set DirName1 = $3 # holder names HXXXX_HXXXY [ if more than one version is present HXXXX_HXXXY_VK]
set DirName2 = $4 # it refers to the Vbias scan we are analizing [ 0 correspond to the lowest Bias ]
#echo $DirName

set Nfiles = $5
set Nsteps = $6 # in this case it refers to the index of the wheel scan we are performing [1-9]
set FileVersion = $7

set ScanIdx = _pos{$Nsteps}

#Nsteps= Nsteps+1
#echo $Nfiles

set DirName = "{$DirName1}/{$DirName2}"
#
# ------------------------------------------------------------------------
# Get runs to be analyzed
# ------------------------------------------------------------------------
#

#set FileDir = /home/daqmu2e/qa_wheel/data/$DirName
set FileDir = /home/user/qa_wheel/data/$DirName
set FileList = `ls -salp $FileDir/wave0*pos{$Nsteps}.dat | awk '{print $10}' | grep wave`
echo "FileList: " $#FileList
echo "FileList: " $FileList

#
# ------------------------------------------------------------------------
# Create output directory
# ------------------------------------------------------------------------
#

  if ( ! -e /home/user/qa_wheel/roottople/$DirName ) then
    cd /home/user/qa_wheel/roottople
    mkdir $DirName1
    mkdir $DirName1/$DirName2
    cd -
  endif

#
# ------------------------------------------------------------------------
# Loop on data...
# ------------------------------------------------------------------------
#

#set i = 0

if ( $#FileList > 0 ) then
    foreach File ( $FileList ) 
#        @ i = $i + 1

        #set SiPMNum = `ls -salp $File | awk '{print $10}' | grep wave | awk -Fwave '{print $2}' | awk -F_ '{print $1}'`
        set FileOut = {$DirName1}{$ScanIdx}
	set FileLog = {$DirName1}_{$DirName2}{$ScanIdx}
	echo  File - FileDir - _posN - FileOut: $File $FileDir $ScanIdx $FileOut


        if ( ! -e /home/user/qa_wheel/roottople/$DirName/$FileOut.root && ! -e /home/user/qa_wheel/log/$FileLog.txt && ! -e /home/user/qa_wheel/log/$FileLog.log ) then

	    echo Creating root file for: $DirName $ScanIdx

#
# ------------------------------------------------------------------------
# Create input file
# ------------------------------------------------------------------------
#
# Number of files to be analyzed
	    
            echo $Nfiles                         > /home/user/qa_wheel/log/$FileLog.txt
# File name(s)
            set i = 0
            while ( $i < $Nfiles )
                set FileIn = {$FileDir}/wave{$i}{$ScanIdx}
	        echo $FileIn                      >> /home/user/qa_wheel/log/$FileLog.txt
      	        echo  $i $Nfiles $FileIn
                @ i = $i + 1
            end
# Output files
echo /home/user/qa_wheel/roottople/$DirName/$FileOut >> /home/user/qa_wheel/log/$FileLog.txt
# Number of events
	    echo 100000                         >> /home/user/qa_wheel/log/$FileLog.txt
#
# ------------------------------------------------------------------------
# Running ROOT in batch mode
# ------------------------------------------------------------------------
#
#	    root -l -b -n -q makeroottopla_specs.C++ < /home/user/qa_wheel/log/$FileOut.txt >& /home/user/qa_wheel/log/$FileOut.log
#	    root -l -b -n -q makeroottopla_fee_bin.C++ < /home/user/qa_wheel/log/$FileOut.txt >& /home/user/qa_wheel/log/$FileOut.log
	    #root -l -b -n -q /home/user/qa_wheel/root/makeroottopla_newV.C++ < /home/user/qa_wheel/log/$FileLog.txt >& /home/user/qa_wheel/log/$FileLog.log
            echo .L /home/user/qa_wheel/root/makeroottopla-10set21_C.so\\n makeroottopla_newV\( \"/home/user/qa_wheel/log/$FileLog.txt\"\) | root > & /home/user/qa_wheel/log/$FileLog.log
	else
	    echo Files already exist for: $DirName $ScanIdx
	endif

    end         # Loop on FileList
endif         # FileList not equal 0
#


echo "finished roottople --------------------------------------------------------------------------------------"

set DirIn  = /home/user/qa_wheel/roottople/$DirName
echo $DirIn
#set DirOut = /home/daqmu2e/qa_fee/hist/$DirName
set DirOut = /home/user/qa_wheel/hist/$DirName
echo $DirOut

set FileList = `ls -salp $DirIn/*{$ScanIdx}*.root | awk '{print $10}'`
echo "Number of files:" $#FileList
echo $FileList
#
# Create output directory
#

  if ( ! -e /home/user/qa_wheel/hist/$DirName ) then
    cd /home/user/qa_wheel/hist
    mkdir $DirName1
    mkdir $DirName1/$DirName2
    cd -
  endif

#
# Loop on data... create histos
#

#goto fit

#set i = 0
########################################check
if ( $#FileList > 0 ) then
    foreach FileIn ( $FileList ) 
#        @ i = $i + 1
#	set FileName = `echo "$FileIn" | awk -F/ '{print$7}' | awk -F.root '{print$1}'`
	set FileName = `echo "$FileIn" | awk -F/ '{print$8}' | awk -F.root '{print$1}'`

        echo "Filename:" $FileName
        set FileOut = $DirOut/{$FileName}_hist.root
	echo "Fileout:" $FileOut
        set MyMacro   = /home/user/qa_wheel/log/{$FileName}_{$DirName2}_hist.C
#        set FileLog = /home/daqmu2e/qa_fee/log/{$DirName}_pos{$NewString}_hist.log
	echo $FileIn $FileOut $MyMacro

        echo Checking if $FileOut and $MyMacro exist
        if ( ! -e $FileOut && ! -e $MyMacro ) then #blocca se esiste gia file o programma

	    echo Creating histos for {$DirName}: $FileOut

#
# Preparing ROOT macro
#
	    echo  \{                                                      > $MyMacro
	    echo  gROOT-\>Reset\(\)\;                                    >> $MyMacro
	    echo  \#include \<TExec.h\>                                  >> $MyMacro
	    echo  \#include \<TString.h\>                                >> $MyMacro
	    echo  \#include \<TFile.h\>                                  >> $MyMacro
#	    echo  \#include \<iostream\>                          >> $MyMacro
            echo                                                         >> $MyMacro
	    echo  TFile\* FileIn = new TFile\(\"$FileIn\"\)\;            >> $MyMacro
            echo                                                         >> $MyMacro
            echo  TExec \*myExec = new TExec\(\)\;                       >> $MyMacro
            echo  myExec-\>SetAction\(\".L qa_fee_newV_C.so\"\)\;         >> $MyMacro
            echo  myExec-\>Exec\(\)\;                                    >> $MyMacro
            echo                                                         >> $MyMacro
            echo  FileIn-\>cd\(\)\;                                      >> $MyMacro
            echo  TString name = FileIn -\> GetName\(\)\;                >> $MyMacro
	    echo  TString H1   = \"$Holder1\"\;                          >> $MyMacro
	    echo  TString H2   = \"$Holder2\"\;                          >> $MyMacro
            echo  myExec-\>SetAction\(\"qa_fee ana\(name\)\"\)\; >> $MyMacro
            echo  myExec-\>Exec\(\)\;                                    >> $MyMacro
            echo  TString nameout = \"$FileOut\"\;                       >> $MyMacro
            echo  myExec-\>SetAction\(\"ana.Loop\(nameout\,H1\,H2\)\"\)\;        >> $MyMacro
            echo  myExec-\>Exec\(\)\;                                    >> $MyMacro
            echo  \}                                                     >> $MyMacro
#
# Running ROOT in batch mode
	    cd /home/user/qa_wheel/root
            root -l -b -n -q $MyMacro >& /home/user/qa_wheel/log/{$FileLog}_hist.log
#	    mv hist.root $FileOut
#
        else
	    echo Files already exist for: $DirName $FileName
        endif
    end         # Loop on FileList
endif         # FileList not equal 0
#

#
# Fit histos
#

#exit(-1)

fit:

echo Fitting histos

set FileDir = /home/user/qa_wheel/hist/$DirName

set FileList = `ls -salp $FileDir/*{$ScanIdx}*full*.root | awk '{print $10}'`
#set FileList = `ls -salp $FileDir/*.root | awk '{print $10}'`
echo $#FileList

echo $FileList ------------------------------
set Numb = "1234567890"

if ( $#FileList > 0 ) then
    foreach FileIn ( $FileList ) 
#        @ i = $i + 1

        set FileName = `echo {$DirName1}{$ScanIdx}full`

        set FileOut = /home/user/qa_wheel/specs/{$FileName}_{$DirName2}.dat
        set FileRoot = /home/user/qa_wheel/specs/{$FileName}_{$DirName2}.root
	echo FileIn - FileOut - ScanIdx: $FileIn $FileOut $ScanIdx

#
# Extract position
#
	set StringLen = `expr length $ScanIdx | awk '{print$1}'`
#	echo $StringLen


        set FileLog = /home/user/qa_wheel/log/{$FileName}_{$DirName2}_fit.log
        set FileInp = /home/user/qa_wheel/log/{$FileName}_{$DirName2}_fit.txt

        echo FileLog, FileInp: $FileLog $FileInp ------------------------------
        if ( ! -e $FileLog && ! -e $FileInp ) then

	    echo Fitting root file: $FileIn

#
# Create input file
#
	    echo $FileIn                  > $FileInp
	    echo $Nsteps                 >> $FileInp
	    echo $FileOut                >> $FileInp
	    echo $FileRoot               >> $FileInp
#
# Running ROOT in batch mode
#
            echo FileInp: $FileInp --------------------

	    root -l -b -n -q fit_fee_final.C < $FileInp >& $FileLog
    #
    #	    sleep 50
	else
	    echo Files already exist for: $DirName $ScanIdx
	endif
	    
    end         # Loop on FileList
endif         # FileList not equal 0


