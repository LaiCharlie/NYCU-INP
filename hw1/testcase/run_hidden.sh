#!/bin/sh

HOST=$1
shift

echo '## 200'
/testcase/200.py $HOST /hidden/.txt				        	                /html/hidden/.txt				                    $@
/testcase/200.py $HOST /hidden/image/../.txt				                /html/hidden/.txt		                            $@
/testcase/200.py $HOST /hidden/image/giphy.gif                              /html/hidden/image/giphy.gif                        $@
/testcase/200.py $HOST /hidden/sPeCI%401_3ncoded_uRl%2B%3DUr1.txt           /html/hidden/sPeCI@1_3ncoded_uRl+=Ur1.txt           $@
/testcase/200.py $HOST /hidden/%e4%b8%ad%e6%96%87%e8%b3%87%e6%96%99%e5%a4%be%2f%e4%b8%ad%e6%96%87%e6%aa%94%e5%90%8d.txt /html/hidden/中文資料夾/中文檔名.txt              $@
/testcase/200.py $HOST /hidden/%e4%b8%ad%e6%96%87%e8%b3%87%e6%96%99%e5%a4%be%2f%e4%b8%ad%e6%96%87%e6%aa%94%e5%90%8d.txt/?SSS=BBB /html/hidden/中文資料夾/中文檔名.txt     $@

echo ''
echo '## NNN'
/testcase/NNN.py $HOST /hidden/%e4%b8%ad%e6%96%87%e8%b3%87%e6%96%99%e5%a4%be/  403   $@

echo ''
echo '## 501'
/testcase/501.py $HOST / TITLE		$@
