#!/bin/tcsh

if( $#argv != 7 ) then
  echo "USAGE >>> script.csh Holder1 Holder2 Holder1_Holder2_vXX Gain_vop/ecc. WheelSteps Nchan Version"
  exit
endif

set Holder1 = $1
set Holder2 = $2
set DirName1 = $3 # holder names HXXXX_HXXXY [ if more than one version is present HXXXX_HXXXY_VK]
set DirName2 = $4 # it refers to the Vbias scan we are analizing [ 0 correspond to the lowest Bias ]
set WheelSteps = $5
set Nchan = $6
set Version = $7

set FileGain = /home/user/qa_wheel/specs/{$DirName1}_{$DirName2}
echo $FileGain

cat /home/user/qa_wheel/specs/{$DirName1}_pos*full_{$DirName2}.dat > $FileGain.dat

#rm /home/user/qa_wheel/specs/{$DirName1}_pos*full_{$DirName2}.dat

echo .L /home/user/qa_wheel/root/concat_hist.C "\n"concat_hist\(\"{$DirName1}\", \"{$DirName2}\", $Nchan, $WheelSteps\) | root # -l -b -q

#rm /home/user/qa_wheel/specs/{$DirName1}_pos*full_{$DirName2}.root


g++ /home/user/qa_wheel/root/fee_gain_Final3.C `root-config --cflags --glibs` -o /home/user/qa_wheel/root/fee_gain.x
/home/user/qa_wheel/root/fee_gain.x $Holder1 $Holder2 $DirName1 $DirName2 $WheelSteps $Nchan 1 $Version

#echo .L /home/user/qa_wheel/root/fee_gain_Final3.C "\n"fee_gain_Final3\(\"{$Holder1}\", \"{$Holder2}\", \"{$DirName1}\", \"{$DirName2}\", $WheelSteps, $Nchan, 1, \"{$Version}\" \) | root # -l -b -q
