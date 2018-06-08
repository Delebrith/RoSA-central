var checkSensors;

app.controller("sensorController", function($scope, $http, $cookies, $interval) {

    $scope.sensorArray;

    function failedGetSensor(response) {
        alert('Błąd! ' + response.status);
    }

    function succesfulGetSensor(response) {
        $scope.sensorArray = response.data;
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


    function failedRefreshSensor(response) {
        alert('Błąd! ' + response.status);
    }

    function succesfulRefreshSensor(response) {
        $scope.sensorArray = response.data;
        console.log($scope.sensorArray);
    }

    $scope.refreshSensor = function (address) {
        var response = $http.get($scope.serverAddress + "/RoSA/sensor/refresh?address=" + encodeURIComponent(address));
        response.then(
            function (response) {
                succesfulRefreshSensor(response);
                getSensors();
            },
            function (response) {
                failedRefreshSensor(response);
            });
    }


    function failedModifySensor(response) {
        alert('Błąd!!' + response.status);
    }

    function succesfulModifySensor(response) {
        alert('Sukces!' + response.status);
    }

    $scope.modifySensor = function (address) {
        var threshold = document.getElementById("thresholdOf" + address).value;
        var response = $http.post(
            $scope.serverAddress + "/RoSA/sensor/modify?address=" + encodeURIComponent(address) +
            "&threshold=" + encodeURIComponent(threshold));
        response.then(
            function (response) {
                succesfulModifySensor(response);
            },
            function (response) {
                failedModifySensor(response);
            });
    }

    function failedModifySensor(response) {
        alert('Błąd!' + response.status);
    }

    function succesfulModifySensor(response) {
        alert('Sukces!' + response.status);
    }

    $scope.addSensor = function () {
        var response = $http.post($scope.serverAddress + "/RoSA/sensor/add?address=" + encodeURIComponent($scope.address)
            + "&threshold=" + encodeURIComponent($scope.threshold));
        response.then(
            function (response) {
                succesfulModifySensor(response);
                $scope.getSensors();
                console.log(response);
            },
            function (response) {
                failedModifySensor(response);
                console.log(response);
            });
    }

    function failedDeleteSensor(response) {
        alert('Błąd!' + response.status);
    }

    function succesfulDeleteSensor(response) {
        alert('Sukces!' + response.status);
        //change $scope.sensorArray
    }

    $scope.deleteSensor = function (address) {
        var response = $http.delete($scope.serverAddress + "/RoSA/sensor?address=" + encodeURIComponent(address));
        response.then(
            function (response) {
                succesfulDeleteSensor(response);
            },
            function (response) {
                failedDeleteSensor(response);
            });
    }


    $interval.cancel(checkSensors);
    checkSensors = $interval(function () {
        var response = $http.get($scope.serverAddress + "/RoSA/sensor");
        response.then(
            function (response) {
                succesfulGetSensor(response);
            },
            function (response) {
                failedGetSensor(response);
            });
    }, 12000);

});