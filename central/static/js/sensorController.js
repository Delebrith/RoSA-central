app.controller("sensorController", function($scope, $http, $cookies, $interval) {

    $scope.sensorArray;

    $scope.checkSensors = $interval(function () {
        var response = $http.get($scope.serverAddress + "/RoSA/sensor");
        response.then(
            function (response) {
                succesfulGetSensor(response);
            },
            function (response) {
                failedGetSensor(response);
            });
    }, 12000);

    function failedGetSensor(response)
    {
        alert('Błąd! ' + response.status);
    }

    function succesfulGetSensor(response)
    {
        $scope.sensorArray = response.data.response;
        console.log($scope.sensorArray);
    }

    $scope.getSensors = function () {
        var response = $http.get($scope.serverAddress + "/RoSA/sensor");
        response.then(
        function (response) {
            succesfulGetSensor(response);
        },
        function (response) {
            failedGetSensor(response);
        });
    }

    function failedModifySensor(response)
    {
        alert('Błąd!!' + response.status);
    }

    function succesfulModifySensor(response)
    {
        alert('Sukces!' + response.status);
    }

    $scope.modifySensor = function (address) {
        var threshold = document.getElementById("thresholdOf" + address).value;
        var response = $http.post(
            $scope.serverAddress + "/RoSA/sensor/modify?address=" + address + "&threshold=" + threshold,
            );
        response.then(
            function (response) {
                succesfulModifySensor(response);
            },
            function (response) {
                failedModifySensor(response);
            });
    }

    function failedModifySensor(response)
    {
        alert('Błąd!' + response.status);
    }

    function succesfulModifySensor(response)
    {
        alert('Sukces!' + response.status);
    }

    $scope.addSensor = function () {
        var response = $http.post($scope.serverAddress + "/RoSA/sensor/add?address=" + $scope.address + "&threshold=" + $scope.threshold);
        response.then(
            function (response) {
                succesfulModifySensor(response);
                $scope.getSensors();
                console.log(reponse);
            },
            function (response) {
                failedModifySensor(response);
                console.log(reponse);
            });
    }

    function failedDeleteSensor(response)
    {
        alert('Błąd!' + response.status);
    }

    function succesfulDeleteSensor(response)
    {
        alert('Sukces!' + response.status);
        //change $scope.sensorArray
    }

    $scope.deleteSensor = function (address) {
        var response = $http.delete($scope.serverAddress + "/RoSA/sensor?address=" + address);
        response.then(
            function (response) {
                succesfulDeleteSensor(response);
            },
            function (response) {
                failedDeleteSensor(response);
            });
    }

});