#!/bin/bash
####################################################################
#depcg.sh ios 2014 Vojtech Sikula xsikul13(at)stud.fit.vutbr.cz (c)#
####################################################################
#zpracování parametrù 
gopt=false
ropt=false
popt=false
dopt=false
FUNCTION_ID=""

while getopts 'gpr:d::' opt; do
  case "$opt" in
    g)
      gopt=true ;;
    r)
      ropt=true
      FUNCTION_ID=$OPTARG ;;
    d)
      dopt=true
      FUNCTION_ID=$OPTARG ;;
    p)
      popt=true ;;
    *)
      exit 1 ;;
    :)
      file=$OPTARG ;;
  esac
done
[ $dopt = "true" ] && [ $ropt = "true" ] && exit 1
for a in $@
do
  file=$1	 
  shift
done
#[ -x "$file" ] || exit 1
#################################################################
#uvodni zpradcovani
fileobj=`objdump -d -j .text $file  | grep "<.*>" | egrep "(>:|callq)"| sed 's/+[^>]*//g' | sed 's/^[^<]*//g'`

### operace s @plt v zavislosti na zadanych prepinacich
[ "$popt" = "false" ] && fileobj=`echo "$fileobj" | sed -e '/@plt/d'`
[ "$gopt" = "true" ] && fileobj=`echo "$fileobj" | sed 's/@plt/_PLT/g'`

#zjisteni zavislosti + defaultni vystup
CALLER=""
CALLEE=""
vystup=""
for slovo in $fileobj; do
	if [ "${slovo#*>}" = ":" ]; then
		 CALLER="${slovo%:}"
	else
		[ "$vystup" = "" ] && vystup="$CALLER _SIPKA_ $slovo"
		[ "$vystup" != "" ] && vystup="$vystup
$CALLER _SIPKA_ $slovo"
		#echo "C _SIPKA_ C $CALLER -> $slovo"
	fi			
done

#vystup dle nastavenych prepinacu
vystup=`echo "$vystup" | sort | uniq | sed 's/<//g' | sed  's/>//g' | sed 's/_SIPKA_/->/g'`
[ "$ropt" = "true" ] && vystup=`echo "$vystup" | grep " $FUNCTION_ID$"`
[ "$dopt" = "true" ] && vystup=`echo "$vystup" | grep "^$FUNCTION_ID "`
if [ "$gopt" = "true" ]; then 
	vystup=`echo "$vystup"| sed 's/$/;/g'` 	  
	vystup="digraph CG {
$vystup
}"
fi
vystup=`echo "$vystup" | sed -e '/^;$/d'`
echo "$vystup"
exit 0