import math


class Filter:

    def __init__(self):
        self.fin = open("octopus2_offset.ngc", "r")
        self.fout = open("octopus2_offset_filtered2.ngc", "w")
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
            return None
            #quit()

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

        count = 0
        p = self.readPath()

        while( not ( p is None) ):

            s = p[3]

            if self.isNewPath(s):
                # print s
                count += 1 
                self.seen.append(s)
                for line in p:
                    self.writeLine(line)

            p = self.readPath()

        print "%d paths left" % count


    def isNewPath(self, p):

        for path in self.seen:
            if self.comparePaths(path, p):
                return False
        else:
            return True


    def comparePaths(self, p1, p2):
        try:
            (op1, x1, y1) = [t(s) for t,s in zip((str,str,str),p1.split())]
            (op2, x2, y2) = [t(s) for t,s in zip((str,str,str),p2.split())]

            if ( abs(float(x1[2:]) - float(x2[2:])) <= 1E-2 and abs(float(y1[2:]) - float(y2[2:])) <= 1E-2 ):
                return True
            else:
                return False

            #print str(float(x[2:])) + ", " + str(float(y[2:]))


        except Exception,e:
            print e
            print "compare error in " + p1 + ", " + p2



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
