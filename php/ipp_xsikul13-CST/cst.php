<?php

#CST:xsikul13

iconv_set_encoding("input_encoding", "ISO-8859-2");
iconv_set_encoding("internal_encoding", "UTF-8");
iconv_set_encoding("output_encoding", "ISO-8859-2");

$long_opts = array( 
	"help",
	"input:",
	"output:",
	"nosubdir",
);
$short_opts = ("koiw:cp");
$o = getopt($short_opts, $long_opts);
 
/*   --help  	*/
if (isset($o['help'])){
	if ($argc == 2){
		help();
		exit(0);
	}else
		printf("CHYBA\nna helpu\n");

}

$prepinac_ok = 1;
$prepinac = "";
$vzor ="";
if (isset($o['k'])) { $prepinac_ok++; $prepinac = "k";}
if (isset($o['o'])) { $prepinac_ok++; $prepinac = "o";}
if (isset($o['i'])) { $prepinac_ok++; $prepinac = "i";}
if (isset($o['w'])) { $prepinac_ok++; $vzor = $o['w']; $prepinac = "w";}
if (isset($o['c'])) { $prepinac_ok++; $prepinac = "c";}
if (!isset($o['k']) && !isset($o['o']) && !isset($o['i']) && !isset($o['w']) && !isset($o['c']) )
	exit(1);
  

if ($prepinac_ok  != 2 ){fprintf(STDERR, "CHYBA - prepinace\n"); exit(1);}

if (isset($o['input'])) { $prepinac_ok++; $input = $o["input"]; 
//	if ($o["input"][0] != "/" || $o["input"][0] == ".") 
//    	$o["input"]=getcwd()."/".preg_replace("/^(\\.\/|\\.)/", "", $o["input"]); 
}else  $o['input'] = getcwd();
  

if (isset($o["output"])) { $prepinac_ok++; $output = $o["output"]; }
else $output = STDOUT; 
 
if (isset($o["p"])) { $prepinac_ok++; $p = TRUE; }

if (isset($o["nosubdir"])) { $prepinac_ok++; if (!is_dir($o["input"])) exit(1); }

if ($prepinac_ok != $argc) exit(1);	
   
/* ------------------------------------ */
/* ---------------nacteni souborů pro analyzu ------------------- */ 
$soubor = array();
//var_dump($o);
if (is_dir($o['input'])) 
{
	if (isset($o["nosubdir"])) {
    	if ($handle = opendir($o["input"])){ //var_dump($o["input"]);
        //This is the correct way to loop over the directory. 
        	while (false != ($name = readdir($handle))) {
          		if((preg_match("!(\\.c$|\\.h$)!", $name ) && !is_dir($name))) {
            		if (is_readable( realpath($o["input"])."/".($name))){
              			$soubor[realpath($o["input"])."/".($name)] = preg_replace("/\\r/", "",file_get_contents(realpath($o["input"])."/".($name)));
            		}else {fprintf(STDERR, "CHYBA - cteni nosubdir %s - nejde cist\n", $name); exit(21);}
          		} 
        	}
      	}
      	closedir($handle);
    }else{  
  
    	$path = realpath($o["input"]);
    	$objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path), RecursiveIteratorIterator::SELF_FIRST);
    	foreach($objects as $name => $object){
        	if((preg_match("!(\\.c$|\\.h$)!", $name ) && !is_dir($name))) {
    			if (is_readable($name)){
            		$soubor[$name] = preg_replace("/\\r/", "", file_get_contents($name));
          		}else {fprintf(STDERR, "CHYBA - cteni rekurzivni %s\n", ($o["input"])); exit(21);}
        	}
    	}
    }
}else{
 // var_dump($o["input"]);
	if (is_readable($o["input"]))
    	$soubor[realpath($o["input"])] = preg_replace("/\\r/", "", file_get_contents($o["input"]));
    else
      	{fprintf(STDERR, "CHYBA - cteni konkretni soubor %s\n", ($o["input"])); exit(2);}
}
/* --------------------------------------------- */
/* --------------- analyza --------------------- */
  
$akce = array();
foreach($soubor as $name => $obsah){
	$akce[$name] = analyza($prepinac, $obsah, $vzor);
    //var_dump($obsah);   
}  
/* ------------------------------------------------------------- */
/* ------------------- serazeni ------------------------ */
$ret = 0;
if (isset($o['p'])) {
	$nazvy = array();
    foreach($akce as $name => $obsah){
      //$nazvy[$name] = bez_dia(preg_replace("!.*/!","",$name );
    	$nazvy[bez_dia(preg_replace("!.*/!","",$name ))] = $obsah;
    }  
    $ret =  vypis($nazvy, $output);
}else{
	foreach($akce as $name => $obsah){
		$nazvy[bez_dia($name)] = $obsah;
	}
	$ret = vypis($nazvy, $output);

}
exit( $ret);
  
/* ------------------------- KONEC ---------------------------- */
/* ------------------------- FUNKCE ---------------------------- */
function analyza($akce, $obsah, $vzor=""){
    switch($akce){
      	case "c": return c($obsah); break;
      	case "k": return k($obsah); break;
      	case "o": return o($obsah); break;
      	case "w": return w($obsah, $vzor); break;
      	case "i": return i($obsah); break;
    }
}
/* ------------------------------------------------------------- */

function help(){ // toto je funkce ktera tiskne na standardni vystup napovedu
	printf( "-------------------------------------------\n".
	        "cst.php - pocita ruzne statistiky zdrojoveho kodu jazyka C\n".
	        "nutnost spustit s jednim z techto parametru:\n \t[ckow:i] dalsi parametry jsou nepovinne\n".
	        "parametry:\n".
	        "   -c - pocet znaku v komentarich\n".
	        "   -k - pocet klicovich slov\n".
	        "   -o - pocet operatoru\n".
	        "   -w=<retezec> - pocet vyskitu retezce\n".
	        "   -i - pocet identifikatoru\n".
	        "   --input=<soubor> analyzovany soubor nebo adresar(defaultne \".\")\n".
	        "   --output=<soubor> vystupni soubor (defaultne stdout)\n".
	        "   --nosubdir - neprohledavají se podadresare\n".
	        "   --help -tato napoveda\n".
	        "   -p - vypis bez absolutni cesty k souboru\n".
	        "podrobnejsi informace lze naleznout v zadani projektu\n".
	        "--------------------------------------------\n"
		);
}	
/* ------------------------------------------------------------- */ 
function vypis($soubor, $output){
    if ($output != STDOUT){ 
	if (file_exists($output)){
     	    if (!is_writable($output) /*&& file_exists($output)*/)
        		exit(3);
           }        	
     	if(!$f = fopen($output, "w")) exit(3);
	    
    
    }else $f = STDOUT;

    if ($soubor == null) {fprintf($f, "CELKEM: 0\n"); exit(0); }
    ksort($soubor);
    $celkem = 0;
    $max = strlen("CELKEM:");
    
    foreach ($soubor as $nazev => $pocet){
       	$max = ($max<strlen($nazev))?strlen($nazev):$max;
      	$celkem += $pocet;
    } 
    
    $max = $max + strlen("$celkem");
    foreach ($soubor as $nazev => $pocet){
      	$s = mezery($max-strlen($nazev)-strlen("$pocet"));
      	$str = sprintf("%s%s %d\n",$nazev, $s , $pocet);
      	fprintf($f, $str);
    }
    $str = sprintf("%s%s %d\n", "CELKEM:",mezery($max-strlen("CELKEM:")-strlen("$celkem")), $celkem);
    fprintf($f, $str);

}
/* ------------------------------------------------------------- */ 
function bez_dia($nazev){// odstrani diakritiku ze zadaneho retezce


$prevodni_tabulka = Array(
  'ä'=>'a',  'Ä'=>'A',  'á'=>'a',  'Á'=>'A',
  'à'=>'a',  'À'=>'A',  'ã'=>'a',  'Ã'=>'A',
  'â'=>'a',  'Â'=>'A',  'č'=>'c',  'Č'=>'C',
  'ć'=>'c',  'Ć'=>'C',  'ď'=>'d',  'Ď'=>'D',
  'ě'=>'e',  'Ě'=>'E',  'é'=>'e',  'É'=>'E',
  'ë'=>'e',  'Ë'=>'E',  'è'=>'e',  'È'=>'E',
  'ê'=>'e',  'Ê'=>'E',  'í'=>'i',  'Í'=>'I',
  'ï'=>'i',  'Ï'=>'I',  'ì'=>'i',  'Ì'=>'I',
  'î'=>'i',  'Î'=>'I',  'ľ'=>'l',  'Ľ'=>'L',
  'ĺ'=>'l',  'Ĺ'=>'L',  'ń'=>'n',  'Ń'=>'N',
  'ň'=>'n',  'Ň'=>'N',  'ñ'=>'n',  'Ñ'=>'N',
  'ó'=>'o',  'Ó'=>'O',  'ö'=>'o',  'Ö'=>'O',
  'ô'=>'o',  'Ô'=>'O',  'ò'=>'o',  'Ò'=>'O', 
  'õ'=>'o',  'Õ'=>'O',  'ő'=>'o',  'Ő'=>'O',
  'ř'=>'r',  'Ř'=>'R',  'ŕ'=>'r',  'Ŕ'=>'R',
  'š'=>'s',  'Š'=>'S',  'ś'=>'s',
  'Ś'=>'S',  'ť'=>'t',  'Ť'=>'T',  'ú'=>'u',
  'Ú'=>'U',  'ů'=>'u',  'Ů'=>'U',  'ü'=>'u',
  'Ü'=>'U',  'ù'=>'u',  'Ù'=>'U',  'ũ'=>'u',
  'Ũ'=>'U',  'û'=>'u',  'Û'=>'U',  'ý'=>'y',
  'Ý'=>'Y',  'ž'=>'z',  'Ž'=>'Z',  'ź'=>'z',
  'Ź'=>'Z');
 
return  strtr($nazev, $prevodni_tabulka);

//	return strtr($nazev,  'áčďéěëíµňôóöŕřšťúůüýžÁÄČĎÉĚËÍĄŇÓÖÔŘŔŠŤÚŮÜÝŽ', 
//		              'acdeeeilnooorrstuuuyzAACDEEEIANOOORRSTUUUYZ');
}   

function mezery($poc){//vraci odpovidajici pocet mezer
    $m ="";
    for($i=0; $i<$poc;$i++)
      	$m = $m." ";
    return $m;
}
/* ------------------------------------------------------------- */

/* ----------------------- KOMENTAREE --------------------------- */
function c($soubor){ // $soubor - analyzovany soubor
    //$size_orig = strlen($soubor);
    //$soubor = preg_replace("/(\\/\\*\\X*?\\*\\/)|(\\/\\/(.*\\\\\\n)*.*\\n)/", "", $soubor, -1);
    //$soubor = strtr($soubor, 	'áčďéěëíµňôóöŕřšťúůüýžÁÄČĎÉĚËÍĄŇÓÖÔŘŔŠŤÚŮÜÝŽ', 
    //							'acdeeeilnooorrstuuuyzaacdeeelinooorrstuuuyz'); // nahrazenĂ­ diakritiiky
    //var_dump($soubor);
    $re = "/(?<makra>^#(?:.*\\\\\\n)*.*\\n?)|(?<komentare>(?:\\/\\*(?:.*\\n)*?.*\\*\\/)|(?:\\/\\/(?:.*\\\\\\n)*.*\\n?))|(?<retezec>\\\"(?:(?:\\\\\")*(?s).*?)*?\\\")|(?<znak>\\'.*?\\')|(?<cisla>(?:0[xX][0-9a-fA-F]+)|(?:[-+]?[0-9]*\\.?[0-9]+(?:[eE][-+]?[0-9]+)?)|(?:[1-9][0-9]*))*/m"; 
    preg_match_all( $re  , $soubor, $matches);
    $pocet = 0;
    foreach ($matches["komentare"] as $kom => $val) {
   		$pocet += strlen($val);
    }
    return $pocet;
}

/* --------------------------------------------------------------- */
/* ----------------------- IDENTIFIKATORY ---------------------- */
function i($soubor){// $soubor - analyzovany soubor
    $pocet_k = k($soubor);
	$re = "/(?<makra>^#(?:.*\\\\\\n)*.*\\n?)|(?<komentare>(?:\\/\\*(?:.*\\n)*?.*\\*\\/)|(?:\\/\\/(?:.*\\\\\\n)*.*\\n?))|(?<retezec>\\\"(?:(?:\\\\\")*(?s).*?)*?\\\")|(?<znak>\\'.*?\\')*/m"; 
																																														// cisla pric																																			
    $soubor = preg_replace($re, "", $soubor, -1 );
//    var_dump($soubor);
// odebrat operĂĄtory literĂĄli

//    $soubor = preg_replace("!(<<=|>>=)!"," ",$soubor,-1, $count );
//    $soubor = preg_replace("/(&=|--|\\/=|<<|==|\\|=|\\+\\+|-=|^=|\\|\\||%=|\\+=|->|<=|>=|>>|&&|\\*=)/", " ",$soubor, -1, $count);
//    $soubor = preg_replace("/(!|\\+|\\^|>|&|\\*|[a-zA-Z0-9_]\\.[a-zA-Z0-9_]|<|=|~|-|\\/)/", " ",$soubor,-1,$count);
 
    $soubor = preg_replace("!([-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)|([1-9][0-9]*)|(0[xX][0-9a-fA-F]*)!", " ",$soubor,-1);
    

    $klic = "[a-zA-Z_][a-zA-Z0-9_]*";
    $pocet = 0;
//    while (1){
      
      $soubor = preg_replace("/".$klic."/"," ",$soubor,-1, $count );
 //     if ($count == 0){break;}
      $pocet += $count;
      
//    }
    return $pocet - $pocet_k;
}
/* ------------------------------------------------------------- */
/* ------------------------- OPERATORY ------------------------- */
function o($soubor){// $soubor - analyzovany soubor
   	$re = "/(?<makra>^#(?:.*\\\\\\n)*.*\\n?)|(?<komentare>(?:\\/\\*(?:.*\\n)*?.*\\*\\/)|(?:\\/\\/(?:.*\\\\\\n)*.*\\n?))|(?<retezec>\\\"(?:(?:\\\\\")*(?s).*?)*?\\\")|(?<znak>\\'.*?\\')/m"; 

    $soubor = preg_replace($re, " ", $soubor, -1 );
                            
    $pocet = 0;
    $soubor = preg_replace("!(<<=|>>=)!"," ",$soubor,-1, $count );
    $pocet = $count; 
    $soubor = preg_replace("/(&=|--|\\/=|<<|==|\\|=|\\+\\+|-=|^=|\\|\\||%=|\\+=|->|<=|>=|>>|&&|\\*=)/", " ",$soubor, -1, $count);
    $pocet += $count;
    $soubor = preg_replace("/(!|\\+|\\^|>|&|[a-zA-Z0-9_]\\.[a-zA-Z0-9_]|<|=|~|-|\\/)/", " ",$soubor,-1,$count);
    $pocet += $count;
    $soubor = preg_replace("/\\s/m", "", $soubor, -1);
    $soubor = preg_replace("/(,|\\.|char|const|double|enum|extern|float|inline|int|long|register|restrict|short|singned|static|struct|typedef|union|unsigned|void|volatile)\\(*\\**/", "", $soubor, -1);
    $soubor = preg_replace("/\\*/", "", $soubor, -1, $count);
    $pocet += $count;
//TODO nepoÄĂ­tam * - nĂĄsobenĂ­ :(

    return $pocet;
  }
/* ------------------------------------------------------------- */

/* ------------------------- HLEDANI --------------------------- */
function w($soubor, $pattern){// $soubor - analyzovany soubor $pattern - hledany retezec
//TODO - upravit $pattern "!" DONE
    return (preg_match_all( "!".preg_quote($pattern, '!')."!"  , $soubor, $nevim) );
    
  }
/* ------------------------------------------------------------- */
/* ---------------------- KLICOVA SLOVA ------------------------ */
function k($soubor){// $soubor - analyzovany soubor
  
    $re = "/(?<makra>^#(?:.*\\\\\\n)*.*\\n?)|(?<komentare>(?:\\/\\*(?:.*\\n)*?.*\\*\\/)|(?:\\/\\/(?:.*\\\\\\n)*.*\\n?))|(?<retezec>\\\"(?:(?:\\\\\")*(?s).*?)*?\\\")|(?<znak>\\'.*?\\')/m"; 

    $soubor = preg_replace($re, " ", $soubor, -1 );
    $klic = "(^|[^a-zA-Z0-9_])(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|inline|int|long|register|restrict|return|short|singned|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)($|[^a-zA-Z0-9_])";
    $pocet = 0;
    while (1){
      
      	$soubor = preg_replace("!".$klic."!"," ",$soubor,-1, $count );
      	if ($count == 0){break;}
      		$pocet += $count;
    }
   
    return $pocet;
}
/* ------------------------------------------------------------- */

/* 
(?<makra>^#(?:.*\\\n)*.*\n?)|(?<komentare>(?:\/\*(?:.*\n)*?.*\*\/)|(?:\/\/(?:.*\\\n)*.*\n?))|(?<retezec>\"(?:(?:\\")*.*)*?\")|(?<znak>\'.*?\')|(?<cisla>(?:0[xX][0-9a-fA-F]+)|(?:[-+]?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?)|(?:[1-9][0-9]*))*/
?>
