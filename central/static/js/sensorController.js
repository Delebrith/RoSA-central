app.controller("sensorController", function($scope, $http, $cookies) {

    function failedGetSensor(response)
    {
        alert('Błąd!');
    }

    function succesfulGetSensor(response)
    {
        alert('Sukces!');
    }

    $scope.getSensors = function () {
        var response = $http.get("/RoSA/sensor");
        response.then(
        function (response) {
            succesfulGetSensor(response);
        },
        function (response) {
            failedGetSensor(response);
        });
    }

    function failedPostSensor(response)
    {
        alert('Błąd!!');
    }

    function succesfulPostSensor(response)
    {
        alert('Sukces!');
    }

    $scope.postSensor = function (address, value) {
        var response = $http.post("/RoSA/sensor?address=" + address + "&value=" + value);
        response.then(
            function (response) {
                succesfulPostSensor(response);
            },
            function (response) {
                failedPostSensor(response);
            });
    }

    function failedPutSensor(response)
    {
        alert('Błąd!');
    }

    function succesfulPutSensor(response)
    {
        alert('Sukces!');
    }

    $scope.putSensor = function (address, value) {
        var response = $http.put("/RoSA/sensor?address=" + address + "&value=" + value);
        response.then(
            function (response) {
                succesfulPutSensor(response);
            },
            function (response) {
                failedPutSensor(response);
            });
    }

    function failedDeleteSensor(response)
    {
        alert('Sukces!');
    }

    function succesfulDeleteSensor(response)
    {
        $window.location.reload();
    }

    $scope.deleteSensor = function (address) {
        var response = $http.delete("/RoSA/sensor?address=" + address);
        response.then(
            function (response) {
                succesfulPostSensor(response);
            },
            function (response) {
                failedPostSensor(response);
            });
    }

});