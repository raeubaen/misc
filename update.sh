git add .
dt=`date '+%d/%m/%Y_%H:%M:%S'`
git commit -m $dt --branch master
git push -u origin master

