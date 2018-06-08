app.controller('logoutController', function ($scope, $http, $cookies, $interval) {
	$scope.logout = function() {

        var response = $http.post($scope.serverAddress + "/RoSA/logout");
        response.then(
            function (response) {
                $cookies.remove('sessionId');
                sessionStorage.clear();
                $scope.checkSensors = undefined;
                $interval.cancel(checkSensors);
                alert("Wylogowano!");
            },
            function (response) {
                $cookies.remove('sessionId');
                sessionStorage.clear();
                $interval.cancel(checkSensors);
                alert("Błąd w czasie wylogowania!");
            });
	}
});