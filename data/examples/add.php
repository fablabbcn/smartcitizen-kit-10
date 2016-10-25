  <?php

    /*

    Smart Data - add.php

    Basic example, reads SmartCitizen request and stored in a csv file.

    You will need to set your end-point in the SCK firmware Constants.h (WEB[] in line 216)

    Example curl:

    $ curl -v -X PUT -H 'Host: data.smartcitizen.me' -H 'User-Agent: SmartCitizen' -H 'X-SmartCitizenMacADDR: 00:00:00:00:00:01' -H 'X-SmartCitizenVersion: 1.1-0.8.5-A' -H 'X-SmartCitizenData: [{"temp":"29090.6","hum":"6815.74","light":"30000","bat":"786","panel":"0","co":"112500","no2":"200000","noise":"2","nets":"10","timestamp":"2015-04-06 10:38:00"}]' 127.0.0.1/data/examples/add.php

    */

    include('../sck_sensor_data.php');
    
    $headers = getallheaders(); 
 
    $data = $headers['X-SmartCitizenData'];
  
    $datapoints = json_decode($data, true);
  
    foreach ($datapoints as $datapoint) {
      $datapoint = SCKSensorData::SCK11Convert($datapoint);
      $csv .=  implode(', ', $datapoint);
    }

    $csv .= PHP_EOL;

    file_put_contents('./data.csv', $csv, FILE_APPEND);

   
  ?>