# Python_hw1
## part.1 & 2
no significant change as the origial code imports the csv file into the list by labeling each data by
it's weather type.
```bash
cwb_filename = '/home/ee2405/ee2405/Python_hw1/108011235.csv'

data = []

header = []

with open(cwb_filename) as csvfile:

   mycsv = csv.DictReader(csvfile)

   header = mycsv.fieldnames

   for row in mycsv:

      data.append(row)
```

Only 2 new lines are added at the end. which is 

```bash
if row['TEMP'] == '-99.000' or row['TEMP'] == '-999.000':
            data.pop()
```

this allows data with false content getting removed from the list.

## part.3

first create a list to carry the data required
naming it data_request
```bash
data_request = []
```
using filter function to choose the data that meet the criteria which the lamda function required. 
```bash
target_data = list(filter(lambda item: item['station_id'] == 'C0A880', data))
```

then set a dummy parameter that is small enough, and using for loop to compare with the required data
if the number is larger than the parameter, then the number will be assign to the parameter.
naming the parameter largest to indicate that is stores the largest data required of the station.
```bash
largest = -99;
for i in range(len(target_data)):
      if float(target_data[i]['TEMP']) > largest:
            largest = float(target_data[i]['TEMP'])
```

after the for loop if the largest still remains -99, then append the sequence[(station_id, "none")] to the list, otherwise, append the sequence [(station_id, largest)] into the list.
```bash
if largest == -99:
      data_request.append([target_data[0]['station_id'],"None"])  
else:    
      data_request.append([target_data[0]['station_id'],largest])

```
repeat it for other 4 stations.

## part.4
print out the result.
```bash
print(data_request)
```
![Screenshot 2021-03-08 201846 jpeg](https://user-images.githubusercontent.com/67352558/110320621-8fe53d80-804b-11eb-8310-f4328d23daa6.jpg)

## Run the code

you can run the code through code editor in the terminal to get the result above.
