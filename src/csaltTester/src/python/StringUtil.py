# Utility class used for string processing

def HasText(line, values):
   retval = False
      
   found = 0
   for data in values:
      if data in line:
         found += 1
         
   if found == len(values):
      retval = True
      
   return retval

def TextAfter(line, values):
   retval = ""
    
   if HasText(line, values):
      loc = 0

      for data in values:
         end = line.find(data, 0) + len(data)
         if end > loc:
            loc = end
            
      retval = line[loc+1:]
       
   return retval
