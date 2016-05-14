
class Filter:

    def __init__(self):
        self.fin = open("octopus2_offset.ngc", "r")
        self.fout = open("octopus2_offset_filtered.ngc", "w")
        self.count = 0 
        self.seen = []

    def close(self):
        self.fin.close()
        self.fout.close()

    def readLine(self):
        pass 
        while(True):

            self.count += 1

            s = self.fin.readline()

            # try:
            #     s = self.fin.readline()
            # except:
            #     print "unable to read line"
            #     return None
            #     quit()

            if not s:
                return None
            if len(s) is 0:
                continue
            elif s[0] is 'G' and s[1] is '0':
                return s




    def writeLine(self, s):
        self.fout.write(s)


    def readPath(self):
        ops = []

        s = self.readLine()
        if s is None:
            return None

        if not ("G00 Z5.000000" in s):
            print "invalid start of path\n"
            print s
            quit()

        ops.append(s)

        s = self.readLine()
        if s is None:
            return None

        while not ("G00 Z5.000000" in s):
            #print s
            ops.append(s)
            s = self.readLine()
            if s is None:
                return None

        #ops.append(s)
        return ops


    def filterPaths(self):

        p = self.readPath()

        while( not ( p is None) ):

            s = p[3]

            if not (s in self.seen):
                print s
                self.seen.append(s)
                for line in p:
                    self.writeLine(line)

            p = self.readPath()



if __name__ == "__main__":

    filter = Filter()

    # while(True):
    #     s = filter.readLine()
    #     if s is None:
    #         break
    #     else:
    #         filter.writeLine(s)

    #print filter.readPath()

    filter.filterPaths()

    filter.close()
    quit()