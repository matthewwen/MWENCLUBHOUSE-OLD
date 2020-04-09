import sqlite3
import json

def pageview_settings():
    try:
        conn = sqlite3.connect("webdata.db")
        c    = conn.cursor()
        c.execute("CREATE TABLE IF NOT EXISTS TABLEVIEW (Id INTEGER PRIMARY KEY, name TEXT, type TEXT)")
        c.execute("SELECT * FROM TABLEVIEW")
        rows = c.fetchall()
        tableview = []
        for item in rows:
            tableview.append(item[1])    

        c.execute("CREATE TABLE IF NOT EXISTS PAGEVIEW(Id INTEGER PRIMARY KEY, ref TEXT, visible INTEGER)")
        c.execute("SELECT * FROM PAGEVIEW")
        rows = c.fetchall()

        pageview = []
        for item in rows:
            pageview.append({"ref": item[1], "visible": True if item[2] == 1 else False})

        def get_item(ref_list, ref):
            for item in ref_list:
                if item["ref"] == ref:
                    return item
            return None


        for item in tableview:
            c.execute("CREATE TABLE IF NOT EXISTS {}(Id INTEGER PRIMARY KEY, ref TEXT, weight INTEGER, body TEXT)".format(item))
            c.execute("SELECT * FROM {}".format(item))
            rows = c.fetchall()
            for prop in rows:
                i = get_item(pageview, prop[1])
                if i is not None:
                    i["weight"] = prop[2]
                    i["html"]   = prop[3]
                    i["parent"] = item

        conn.commit()
        conn.close()

        done_json = {"data": pageview}
        return json.dumps(done_json)
    except:
        return None