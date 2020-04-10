import json
import sqlite3
import re
import copy

# algo
def strcmp(str1, str2, inst, dist=0, count=-1, index=-1, queryAtIndex=False):
    if inst == 0: 
        count = count if count != -1 else len(str1) if len(str1) >= len(str2) else len(str2)
        i = 0
        for i in range(count):
            if len(str1) == i or len(str2) == i:
                return 0
            elif str1[i] != str2[i]:
                diff = ord(str2[i]) - ord(str1[i])
                return diff if dist == 0 else -1 * diff 
        return 0
    else:
        return str2 - str1 if dist == 0 else str1 - str2

def swap(data, i1, i2):
    temp = data[i1]
    data[i1] = data[i2]
    data[i2] = temp

def binary_search(data, searchVal, queryList, start=0, end=-1, index=-1):
    end = len(data) if end == -1 else end
    midIdx = int((start + end) / 2)
    obj = data[midIdx]
    diff = strcmp(obj["name"], searchVal, 0, dist=0, count=len(searchVal), index=index)
    if diff == 0:
        # go backwards
        foundIdx = midIdx
        while (foundIdx + 1) > 0 and strcmp(data[foundIdx]["name"], searchVal, 0, \
                dist=0, queryAtIndex=True, count=len(searchVal)) == 0:
            foundIdx -= 1
        foundIdx += 1

        # append forward
        while foundIdx < end and strcmp(data[foundIdx]["name"], searchVal, 0, \
                dist=0, queryAtIndex=True, count=len(searchVal)) == 0:
            queryList.append(data[foundIdx])
            foundIdx+=1
        sort_function(queryList, "pop", dist=1)
    elif diff > 0:
        # go right
        if (end - midIdx) > 1:
            binary_search(data, searchVal, queryList, start=midIdx, end=end, index=index);
    elif diff < 0:
        # go left
        if (midIdx - start) > 1:
            binary_search(data, searchVal, queryList, start=start, end=midIdx, index=index);

def get_obj(data, idVal, start=0, end=-1):
    end = len(data) if end == -1 else end
    end = len(data) if end == -1 else end
    midIdx = int((start + end) / 2)
    obj = data[midIdx]
    diff = int(idVal - obj["id"])
    if diff == 0:
        return obj
    elif diff > 0:
        # go right
        if (end - midIdx) > 1:
            return get_obj(data, idVal, start=midIdx, end=end);
    elif diff < 0:
        # go left
        if (midIdx - start) > 1:
            return get_obj(data, idVal, start=start, end=midIdx);
    return None
    
# use a min heap
def sort_function(data, key, dist=0, query=-1, index=-1):
    numElement = len(data)
    inst = 1 if numElement > 0 and isinstance(data[0][key], int) else 0

    # Time Complexity: O(n + nlog(n)) -> O(n log(n))
    for size in range(numElement):
        element = data[size]
        canContinue = True

        i = size
        while canContinue:
            parent = int((i - 1) / 2)
            canContinue = i != parent and strcmp(data[i][key], data[parent][key], inst, dist=dist, index=index) < 0
            if canContinue:
                swap(data, i, parent)
            i = parent

    # Time Complexity: O(n + query * log(n)) -> O(query * log(n))
    newList = []
    newSize = numElement if query == -1 else query
    for i in range(newSize):
        idx = 0 
        limit = numElement - i - 1
        if query != -1:
            newList.append(data[idx])
        swap(data, idx, limit)

        canContinue = True
        while canContinue:
            # Determine which elements to swap
            left  = 2 * idx + 1
            right = left + 1
            pos = left if left < limit and strcmp(data[idx][key], data[left][key], inst, dist=dist, index=index) > 0 else idx
            pos = right if right < limit and strcmp(data[pos][key], data[right][key], inst,dist=dist, index=index) > 0 else pos
            canContinue = pos != idx  

            # Perform the swap
            if canContinue:
                swap(data, idx, pos)
            idx = pos
    
    return data if len(newList) == 0 else newList

# stralogo
def change_str(index, str1):
    word1 = re.split(r'\s|-', str1)
    if index < len(word1):
        count = 0
        for i in range(index):
            count += len(word1[i]) + 1
        return str1[count::] 
    else:
        return None

def change_list(index, jlist):
    newList = []
    for item in jlist:
        item["name"] = change_str(index, item["name"])
        if item["name"] != None:
            newList.append(item)
    return newList

# main
class PROGRAM_LIST:
    REF_LIST:json
    SORTED_LIST:list
    POP_LIST:json
    MAX_WORD:int

def update_helper(limit=False):
    REF_LIST = None
    try:
        with open('txt/collegeName.txt') as json_file:
            REF_LIST = json.load(json_file)

        MAX_WORD = 0
        for item in REF_LIST["data"]:
            word = re.split(r'\s|-', item["name"])
            MAX_WORD = MAX_WORD if len(word) < MAX_WORD else len(word)

        SORTED_LIST = []
        
        num_query = MAX_WORD
        for i in range(num_query):
            temp = copy.deepcopy(REF_LIST)
            temp["data"] = change_list(i, temp["data"])
            temp["data"] = sort_function(temp["data"], "name")
            SORTED_LIST.append(temp)

        POP_LIST = copy.deepcopy(REF_LIST)
        POP_LIST["data"] = sort_function(POP_LIST["data"], "pop", dist=1)

        return [REF_LIST, SORTED_LIST, POP_LIST, MAX_WORD]
    except:
        return [None, None, None, 0]

QUERY_LIMIT = 100
def init_module():
    [PROGRAM_LIST.REF_LIST, PROGRAM_LIST.SORTED_LIST, PROGRAM_LIST.POP_LIST, PROGRAM_LIST.MAX_WORD] = update_helper()


def get_college():
    result = None
    if PROGRAM_LIST.POP_LIST == None:
        result = {"error", "Database Failure"}
    else:
        returnResult = []
        for index, item in zip(range(QUERY_LIMIT), PROGRAM_LIST.POP_LIST["data"]):
            returnResult.append({"id": item["id"], "name": item["name"], "online": item["online"]})
        result = returnResult
    return json.dumps(result)

def get_detail(collegeid: int):
    result = None
    if PROGRAM_LIST.REF_LIST == None:
        result =  {"error", "Database Failure"}
    else:
        result = get_obj(PROGRAM_LIST.REF_LIST["data"], collegeid)
    return json.dumps(result)

def process_post(collegeid: int, msg: str, href: str, online: bool):
    class hrefContent():
        collegeid: int = 1
        msg: str = ""
        href: str = ""
        online: bool = True

        def __init__(self, collegeid: int, msg: str, href:str, online: bool):
            self.collegeid = collegeid
            self.msg       = msg 
            self.href      = href
            self.online    = online

    college = hrefContent(collegeid, msg, href, online)
    result = None
    try:
        conn = sqlite3.connect("webdata.db")
        c    = conn.cursor()
        c.execute("CREATE TABLE IF NOT EXISTS EXP_REQUEST (id INTEGER, msg TEXT, href TEXT, online INTEGER)")
        insertStr = "INSERT INTO EXP_REQUEST (id, msg, href, online) VALUES({},\"{}\",\"{}\",{});".format(college.collegeid, college.msg, college.href, 1 if college.online else 0)
        c.execute(insertStr)
        conn.commit()
        conn.close()
        result = {"success":True} 
    except:
        result = {"success":False} 
    return json.dumps(result)

def search_query(query: str):
    result = None
    try:
        query = query.lower()
        queryList = []
        for i, listItem in zip(range(len(PROGRAM_LIST.SORTED_LIST)), PROGRAM_LIST.SORTED_LIST):
            tempList = []
            binary_search(listItem["data"], query, tempList, index=i)

            for temp in tempList:
                inQuery = False
                for queryItem in queryList:
                    inQuery = inQuery or queryItem["id"] == temp["id"]
                if not inQuery: 
                    parentObj = get_obj(PROGRAM_LIST.REF_LIST["data"], temp["id"])
                    if parentObj != None:
                        queryList.append(parentObj)


        result = sort_function(queryList, "pop", dist=1)
    except:
        result = {"success": False} 
    
    return json.dumps(result)

def update_memory(temp: str):
    result = None
    if temp == "mwen-kushal":
        [PROGRAM_LIST.REF_LIST, PROGRAM_LIST.SORTED_LIST, PROGRAM_LIST.POP_LIST, PROGRAM_LIST.MAX_WORD] = update_helper(limit=True)
        result =  {"updated":True}
    else:
        result = {"updated":False}
    return json.dumps(result)

# init_module()
# print(get_college())
# print(get_detail(1))
# # print(process_post(1, "test", "https://google.com", True))
# asyncio.run(search_query("Austin"))
# # print(update_memory("mwen-kushal"))
