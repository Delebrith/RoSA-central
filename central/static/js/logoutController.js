app.controller('logoutController', function($scope, $http, $cookies) {
	$scope.logout = function() {

        alert($scope.serverAddress);
        var response = $http.post($scope.serverAddress + "/RoSA/logout");
        response.then(
            function (response) {
                $cookies.remove('session_id');
                alert("Wylogowano!");
            },
            function (response) {
                alert("Błąd w czasie wylogowania!");
            });
	}
});