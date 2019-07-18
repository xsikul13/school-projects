#projekt do SNT 2018 - autor Vojtěch Šikula (xsikul13)
#%reload_ext snt_data
import sys
from snt_data import *

print("#testování celého běhu nad daty sprint\n#pět běhů po 10s pro každý soubor\n#celkový čas cca 10h\n")

#Cases HMS HMCR PAR
cases_parameters = [
    (10, 0.99, 0.1),
    (30, 0.99, 0.1),
    (50, 0.99, 0.1),
    (10, 0.90, 0.1),
    (10, 0.95, 0.1),
    (10, 0.99, 0.1),
    (10, 0.99, 0.0),
    (10, 0.99, 0.1),
    (10, 0.99, 0.4),
    (10, 0.99, 0.7)]

fhiden = [
    "sprint_hidden01.xml",
    "sprint_hidden02.xml",
    "sprint_hidden03.xml",
    "sprint_hidden04.xml",
    "sprint_hidden05.xml",
    "sprint_hidden06.xml",
    "sprint_hidden07.xml",
    "sprint_hidden08.xml",
    "sprint_hidden09.xml",
    "sprint_hidden10.xml"]
fsprint = [
    "sprint01.xml",
    "sprint02.xml",
    "sprint03.xml",
    "sprint04.xml",
    "sprint05.xml",
    "sprint06.xml",
    "sprint07.xml",
    "sprint08.xml",
    "sprint09.xml",
    "sprint10.xml"]


cases20 = list()
#f = open("results", "w")

for c in cases_parameters:
    
    print("parametery: (HMS, HMCR, PAR)", c)
    res = list()
    for r in range(len(fhiden)):
        print("soubor: ", fsprint[r])
        print("(počet iterací, nejlepší nalezené řešení)")
        res.append([])
        #f.write("#"+fsprint[r]+"\n")
        for i in range(5):
            #log, best = run("../sprint/"+fsprint[r],
            log, best = run(("./sprint_data/"+fsprint[r]),
                                TIME= 10, global_best=False, HMS=c[0],
                                HMCR=c[1], PAR=c[2], log_space=100)
            
            print(log[-1][0:2])
            res[r].append(log[-1])
    cases20.append(res)
