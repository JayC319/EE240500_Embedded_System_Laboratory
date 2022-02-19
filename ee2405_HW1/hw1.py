# Part. 1

#=======================================

# Import module

#  csv -- fileIO operation

import csv

#=======================================


# Part. 2

#=======================================

# Read cwb weather data

cwb_filename = '/home/ee2405/ee2405/Python_hw1/108011235.csv'

data = []

header = []

with open(cwb_filename) as csvfile:

   mycsv = csv.DictReader(csvfile)

   header = mycsv.fieldnames

   for row in mycsv:

      data.append(row)
      if row['TEMP'] == '-99.000' or row['TEMP'] == '-999.000':
            data.pop()

#=======================================


# Part. 3

#=======================================

data_request = []


target_data = list(filter(lambda item: item['station_id'] == 'C0A880', data))
largest = -99;
for i in range(len(target_data)):
      if float(target_data[i]['TEMP']) > largest:
            largest = float(target_data[i]['TEMP'])
      
if largest == -99:
      data_request.append([target_data[0]['station_id'],"None"])  
else:
      data_request.append([target_data[0]['station_id'],largest])  


target_data = list(filter(lambda item: item['station_id'] == 'C0F9A0', data))
largest = -99;
for i in range(len(target_data)):
      if float(target_data[i]['TEMP']) > largest:
            largest = float(target_data[i]['TEMP'])
if largest == -99:
      data_request.append([target_data[0]['station_id'],"None"])  
else:    
      data_request.append([target_data[0]['station_id'],largest])


target_data = list(filter(lambda item: item['station_id'] == 'C0G640', data))
largest = -99;
for i in range(len(target_data)):
      if float(target_data[i]['TEMP']) > largest:
            largest = float(target_data[i]['TEMP'])
      
if largest == -99:
      data_request.append([target_data[0]['station_id'],"None"])  
else:
      data_request.append([target_data[0]['station_id'],largest])


target_data = list(filter(lambda item: item['station_id'] == 'C0R190', data))
largest = -99;
for i in range(len(target_data)):
      if float(target_data[i]['TEMP']) > largest:
            largest = float(target_data[i]['TEMP'])
if largest == -99:
      data_request.append([target_data[0]['station_id'],"None"])  
else:   
      data_request.append([target_data[0]['station_id'],largest])


target_data = list(filter(lambda item: item['station_id'] == 'C0X260', data))
largest = -99;
for i in range(len(target_data)):
      if float(target_data[i]['TEMP']) > largest:
            largest = float(target_data[i]['TEMP'])
if largest == -99:
      data_request.append([target_data[0]['station_id'],"None"])  
else:      
      data_request.append([target_data[0]['station_id'],largest])



#=======================================


# Part. 4

#=======================================

# Print result

print(data_request)


#========================================