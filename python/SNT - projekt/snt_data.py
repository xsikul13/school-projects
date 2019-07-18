import random
from datetime import datetime, timedelta
import xmltodictlocal.xmltodict as xmltodict


class rst_data():
    
    def __init__(self, doc):
        self.interval = Interval(doc)
        self.contracts = Contracts(doc)
        self.patterns = Patterns(doc)
        self.shiftTypes = ShiftTypes(doc)
        self.skills = Skills(doc)
        self.employees = Employees(doc)
        self.cover = Cover(doc)
        self.dayOffRequests = DayOffRequests(doc,self.employees)
        self.shiftOffRequests = ShiftOffRequests(doc,self.employees)

#############################################################################
def random_solution(rst):
    jedinec = [[] for i in rst.employees]
    for i in jedinec:
        for ii in range(rst.interval[1]):
            i.append(8)

    #print(jedinec)
    for j in range(rst.interval[1]):
            day = rst.cover[(rst.interval[0]+j) % 7][1]

            for s in range(len(day)):
                x = 0
                while(x < day[s][1]):
                    n = random.randint(0, len(rst.employees)-1)
                    if jedinec[n][j] == 8:
                        jedinec[n][j] = s
                        x += 1
    return jedinec

#############################################################################
def penalty_max_total(e, c):
    #print(c)
    #print(e)
    sum = 0
    for i in e:
        if i != 8:
            sum += 1;
    if sum > c[2]:
        return (sum - c[2])*c[1] #pocetnad * weight 
    else:
        return 0

def penalty_min_total(e, c):
    #print(c)
    #print(e)
    sum = 0
    for i in e:
        if i != 8:
            sum += 1;
    if c[2] - sum  > 0:
        return (c[2] - sum)*c[1] #pocetnad * weight 
    else:
        return 0
def penalty_max_consecutive(e, c):
    #print(c)
    #print(e)
    consecutive = 0
    pen = 0
    for i in e:
        if i != 8:
            consecutive += 1;
            if consecutive > c[2]:
                pen += c[1] # add weight
        else:
            consecutive = 0
    return pen

def penalty_min_consecutive(e, c):
    #print(c)
    #print(e)
    consecutive = 0
    pen = 0
    for i in e:
        if i != 8:
            consecutive += 1;
        else:
            if consecutive == 0: # 
                continue
            else:
                if (consecutive < c[2]):
                    pen += (c[2]-consecutive)*c[1] # kolik chybí do minima * weight
            consecutive = 0
    return pen

def penalty_max_consecutive_free_days(e, c):
    #print(c)
    #print(e)
    consecutive = 0
    pen = 0
    for i in e:
        if i == 8:
            consecutive += 1;
        else:
            if consecutive == 0: # 
                continue
            else:
                if (consecutive > c[2]):
                    pen += (consecutive - c[2])*c[1] # kolik free days nad limint * weight
            consecutive = 0
    return pen

def penalty_min_consecutive_free_days(e, c):
    #print(c)
    #print(e)
    consecutive = 0
    pen = 0
    for i in e:
        if i == 8:
            consecutive += 1;
        else:
            if consecutive == 0: # 
                continue
            else:
                if (consecutive < c[2]):
                    #print(consecutive, c[2], c[1])
                    pen += (c[2] - consecutive)*c[1] # kolik free days do minima * weight
            consecutive = 0
    return pen
##
## weekendy
##    
def penalty_day_off_requests(e, r):
    #print(r)
    #print(e)
    pen = 0
    for i in r:
        
        if (e[i[0]-1] != 8):  #-1 corekce na 1den v měsici na index 0
            #print(i)
            pen += i[1]
    return pen


def penalty_shift_off_requests(e, r,shiftTypes):
    #print(r)
    #print(e)
    pen = 0
    for s in r:
        if (e[s[1]-1] == 8):
            continue #all day off
        if (shiftTypes[e[s[1]-1]][0] == s[0]):  #-1 corekce na 1den v měsici na index 0
            #print(s)
            pen += s[2]
    return pen

def eval_em(rst, e, c, ei): #employee_roast, contract, employee_index
    pen = 0
    pen += penalty_max_total(e,c[1])
    pen += penalty_min_total(e,c[2])
    pen += penalty_max_consecutive(e,c[3])
    pen += penalty_min_consecutive(e,c[4])
    pen += penalty_max_consecutive_free_days(e,c[5])
    pen += penalty_min_consecutive_free_days(e,c[6])
    
    
    pen += penalty_day_off_requests(e, rst.dayOffRequests[ei])
    pen += penalty_shift_off_requests(e, rst.shiftOffRequests[ei], rst.shiftTypes)
    
    return pen

def eval_emp_day(rst, e, c, ei, day): #employee_roast, contract, employee_index
    pen = 0
    #pen += penalty_max_total(e,c[1])
    #pen += penalty_min_total(e,c[2])
    pen += penalty_max_consecutive(e,c[3],day)
    pen += penalty_min_consecutive(e,c[4],day)
    pen += penalty_max_consecutive_free_days(e,c[5],day)
    pen += penalty_min_consecutive_free_days(e,c[6],day)
    
    
    pen += penalty_day_off_requests(e, rst.dayOffRequests[ei], day)
    pen += penalty_shift_off_requests(e, rst.shiftOffRequests[ei], rst.shiftTypes,day)
    
    return pen

def eval_roast(rst, roast):
    penalty = 0
    for i in range(len(rst.employees)):
        penalty += eval_em(rst, roast[i], rst.contracts[rst.employees[i][0]], i)
    return penalty

#############################################################################
#############################################################################

def Interval(doc):
    from datetime import date
    d = doc["SchedulingPeriod"]["StartDate"]
    d1 = date(int(d[:4]), int(d[5:7]), int(d[-2:]))
    ds = doc["SchedulingPeriod"]["EndDate"]
    d2 = date(int(ds[:4]), int(ds[5:7]), int(ds[-2:]))
    interval = (d1.weekday(), d2.day, d)
    return interval

#############################################################################

def Contracts(doc):
    ## make contracts
    contracts = []
    for c in doc["SchedulingPeriod"]["Contracts"]["Contract"]:
        s0 = (int(c["SingleAssignmentPerDay"]["@weight"]), 1) ##
        s1 = (
            int(c["MaxNumAssignments"]["@on"]) ,
            int(c["MaxNumAssignments"]["@weight"]),
            int(c["MaxNumAssignments"]["#text"]),
            "MaxNumAssignments"
             )
        s2 = (
            int(c["MinNumAssignments"]["@on"]) ,
            int(c["MinNumAssignments"]["@weight"]),
            int(c["MinNumAssignments"]["#text"]),
            "MinNumAssignments"
         )
        s3 = (
            int(c["MaxConsecutiveWorkingDays"]["@on"]) ,
            int(c["MaxConsecutiveWorkingDays"]["@weight"]),
            int(c["MaxConsecutiveWorkingDays"]["#text"]),
            "MaxConsecutiveWorkingDays"
         )
        s4 = (
            int(c["MinConsecutiveWorkingDays"]["@on"]) ,
            int(c["MinConsecutiveWorkingDays"]["@weight"]),
            int(c["MinConsecutiveWorkingDays"]["#text"]),
            "MinConsecutiveWorkingDays"
         )
        s5 = (
            int(c["MaxConsecutiveFreeDays"]["@on"]) ,
            int(c["MaxConsecutiveFreeDays"]["@weight"]),
            int(c["MaxConsecutiveFreeDays"]["#text"]),
            "MaxConsecutiveFreeDays"
         )
        s6 = (
            int(c["MinConsecutiveFreeDays"]["@on"]) ,
            int(c["MinConsecutiveFreeDays"]["@weight"]),
            int(c["MinConsecutiveFreeDays"]["#text"]),
            "MinConsecutiveFreeDays"
         )
        s7 = (
            int(c["MaxConsecutiveWorkingWeekends"]["@on"]) ,
            int(c["MaxConsecutiveWorkingWeekends"]["@weight"]),
            int(c["MaxConsecutiveWorkingWeekends"]["#text"]),
            "MaxConsecutiveWorkingWeekends"
         )
        s8 = (
            int(c["MinConsecutiveWorkingWeekends"]["@on"]) ,
            int(c["MinConsecutiveWorkingWeekends"]["@weight"]),
            int(c["MinConsecutiveWorkingWeekends"]["#text"]),
            "MinConsecutiveWorkingWeekends"
         )
        s9 = (
            int(c["MaxWorkingWeekendsInFourWeeks"]["@on"]) ,
            int(c["MaxWorkingWeekendsInFourWeeks"]["@weight"]),
            int(c["MaxWorkingWeekendsInFourWeeks"]["#text"]),
            "MaxWorkingWeekendsInFourWeeks"
         )    
        s10 = (c["WeekendDefinition"],
            "WeekendDefinition")

        s11 = (
            int(c["CompleteWeekends"]["@weight"]),
            1 if (c["CompleteWeekends"]["#text"]== "true") else 0,
            "CompleteWeekends"
         )    
        s12 = (
            int(c["IdenticalShiftTypesDuringWeekend"]["@weight"]),
            1 if (c["IdenticalShiftTypesDuringWeekend"]["#text"]== "true") else 0,
            "IdenticalShiftTypesDuringWeekend"
         )    
        s13 = (
            int(c["NoNightShiftBeforeFreeWeekend"]["@weight"]),
            1 if (c["NoNightShiftBeforeFreeWeekend"]["#text"]== "true") else 0,
            "NoNightShiftBeforeFreeWeekend"
         )    
        s14 = tuple()
        if "TwoFreeDaysAfterNightShifts" in c:
            s14 = (
                int(c["TwoFreeDaysAfterNightShifts"]["@weight"]),
                1 if (c["TwoFreeDaysAfterNightShifts"]["#text"]== "true") else 0,
                "TwoFreeDaysAfterNightShifts"
             )
        else:
            s14 = (0,0)
        s15 = (
            int(c["AlternativeSkillCategory"]["@weight"]),
            1 if (c["AlternativeSkillCategory"]["#text"]== "true") else 0,
            "AlternativeSkillCategory"
         )
        un = []
        for p in c["UnwantedPatterns"]["Pattern"]:
            un.append(int(p))

        contracts.append((s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,un))
    return contracts


#############################################################################

def Patterns(doc):
    # weight, [patternentries]
    patterns = []
    for i in doc["SchedulingPeriod"]["Patterns"]["Pattern"]:
        s0 = int(i["@weight"])
        s1 = [] # PatternEntries
        for ii in i["PatternEntries"]["PatternEntry"]:
            s1.append((ii["ShiftType"], ii["Day"]))


        patterns.append((s0,s1))
    return patterns

#############################################################################
def ShiftTypes(doc):
    shiftTypes = []
    for i in doc["SchedulingPeriod"]["ShiftTypes"]["Shift"]:
        s0 = i["@ID"]
        s1 = i["Skills"]["Skill"] # skills (nurse, headn)
        # resim pouze jeden skill
        #for ii in i["Skills"]:
        #    s1.append(ii)


        shiftTypes.append((s0,s1))
    return shiftTypes

#############################################################################
def Skills(doc):
    return ["Nurse"] # na pevno

def Employees(doc):
    employees = []
    for i in doc["SchedulingPeriod"]["Employees"]["Employee"]:
        s0 = int(i["ContractID"])
        s1 = int(i["Name"])
        s2 = "Nurse" #na pevno   

        employees.append((s0,s1,s2))
    return employees
#############################################################################

def Cover(doc):
    cover = []
    for i in doc["SchedulingPeriod"]["CoverRequirements"]["DayOfWeekCover"]:
        s0 = i["Day"]
        s1 = []
        for ii in i["Cover"]:
            s1.append( (ii["Shift"], int(ii["Preferred"])))

        cover.append((s0,s1))
    return cover
#############################################################################

def DayOffRequests(doc,employees):
    dayOffRequests = [] # (den, weight)
    for i in range(len(employees)):
        dayOffRequests.append([])

    for i in doc["SchedulingPeriod"]["DayOffRequests"]["DayOff"]:
        s0 = int(i["Date"][-2:])
        s1 = int(i["@weight"])
        e = int(i["EmployeeID"])
        dayOffRequests[e].append((s0,s1))
    return dayOffRequests

#############################################################################

def ShiftOffRequests(doc,employees):
    shiftOffRequests = []
    for i in range(len(employees)):
        shiftOffRequests.append([])

    for i in doc["SchedulingPeriod"]["ShiftOffRequests"]["ShiftOff"]:
        s0 = i["ShiftTypeID"]
        s1 = int(i["Date"][-2:])
        s2 = int(i["@weight"])
        e = int(i["EmployeeID"])
        shiftOffRequests[e].append((s0,s1,s2))
    return shiftOffRequests


######################x
#######################



def mem_consideration(rn, HM, j,rand=False):
    i = 0
    #rand = False
    HMS = len(HM)
    if rand:
        rnd = random.randint(0,(HMS-1)*(HMS/2))

        s = HMS
        #print(rand, "rand")
        while rnd > s:
            i +=1
            rnd -= s
            s -= 1
    else:
        i = random.randint(0, HMS-1)
    #print(i)
    for e in range(len(rn)):
        rn[e][j] = HM[i][0][e][j] # přiřazení směny zaměstnanci()
        
def rnd_consideration(rst, rn, j):
    for e in rn:
        e[j] = 8
        
    day = rst.cover[(rst.interval[0]+j) % 7][1]
    for s in range(len(day)):
        x = 0
        while(x < day[s][1]):
            n = random.randint(0, len(rst.employees)-1)
            if rn[n][j] == 8:
                rn[n][j] = s
                x += 1
                
                
def moveOneShift(rn, j):
    e = random.randint(0, len(rn)-1)
    e2 = random.randint(0, len(rn)-1)
    swap = rn[e][j]
    rn[e][j] = rn[e2][j]
    rn[e2][j] = swap
    #r = random.randint(0, len(rn)*len(rn[0])) #emloyees * days
    
def swapOneShift(rn, j):
    e = random.randint(0, len(rn)-1)
    e2 = random.randint(0, len(rn)-1)
    swap = rn[e][j]
    rn[e][j] = rn[e2][j]
    rn[e2][j] = swap    

def swap2Shifts(rn, j):
    e = random.randint(0, len(rn)-1)
    e2 = random.randint(0, len(rn)-1)
    d = random.randint(0, len(rn[0])-1)
    
    swap = rn[e][j]
    rn[e][j] = rn[e2][j]
    rn[e2][j] = swap 
    swap = rn[e][d]
    rn[e][d] = rn[e2][d]
    rn[e2][d] = swap
    
def tokenRingMove(rn, j):
    first = rn[0][j]
    emp = len(rn)
    for i in range(emp-1):
        #if (rn[i][j] != 8): # mají přidělenou směnu
        rn[i][j] = rn[i+1][j]
    rn[emp-1][j]=first

#step1 Initialize the parameters of the nrp and hsa
#%reload_ext autoreload
#from snt_data import *



def run(fileName, global_best=True, TIME=25, HMS=30,HMCR=0.9, PAR = 0.2,log_space=100):
    
    import random
    
    #fileName = "../sprint_hidden_xml/sprint_hidden08.xml" 
    # import xmltodictlocal.xmltodict as xmltodict
    doc = xmltodict.parse(open(fileName, "r").read())
    
    end_time = datetime.now() + timedelta(seconds=TIME)
    pen = []
    

    r = rst_data(doc)


    #step2 - Initialize the harmony memory
    #HMS harmony memory size 50
    #HM harmony memory 
    #HMCR harmony memory consideration rate 
    #        // poměr of selction alocation from hm roaster
    #PAR pich adjusting rate  // pravděpodobnot úpravy přiřazení
    #NI number of iteration 
    #HMCR = 0.90 #[0-1]
    #PAR = 0.2 #[0-1]
    #NI = 10000

    #HMS = 30
    HM = []
    for i in range(HMS):
        roast = random_solution(r)
        HM.append((roast, eval_roast(r,roast)))

    HM = sorted(HM, key=lambda r: r[1])    

    ## vytvoření nového řešení
    
    #for i in range(NI):
    ni = 0
    while datetime.now() < end_time:
        
        #step3 Improvize a new harmony roster

        rn = random_solution(r)
        for j in range(len(rn[0])): #každej den
            if random.uniform(0,1) < HMCR: #memory consideration
                mem_consideration(rn, HM, j, rand=global_best)
            else: ## random consideration 1-HMCR
                rnd_consideration(r,rn,j)
        # memory consideration

        # random consideration

        # pitch adjustment
         #pich adjustment
        for j in (range(len(rn[0]))):
            rand = random.uniform(0,1)
            if rand <= PAR/8:
                moveOneShift(rn, j)
            elif rand <= 2*PAR/8:
                swapOneShift(rn, j)
            elif rand <= 3*PAR/8:
                tokenRingMove(rn, j)
            elif rand <= 4*PAR/8:
                swap2Shifts(rn, j)
#            elif rand <= 5*PAR/8:
#               moveOneShift(rn, j)
#            elif rand <= 6*PAR/8:
#                swapOneShift(rn, j)
#            elif rand <= 7*PAR/8:
#                tokenRingMove(rn, j)
#            elif rand <= 8*PAR/8:
#                swap2Shifts(rn, j)
        # update HM new < worst -> replace   


        cost = eval_roast(r, rn)
        if HM[HMS-1][1] > cost:
            HM[HMS-1] = (rn, cost)
            HM = sorted(HM, key=lambda r: r[1])  
        if ni % log_space == 0:
            #print("best ", HM[0][1], " worst ", HM[HMS-1][1])
            pen.append((ni, HM[0][1],HM[HMS-1][1]))
            
        
        ni += 1
    pen.append((ni, HM[0][1],HM[HMS-1][1]))
    return pen, HM[0]
