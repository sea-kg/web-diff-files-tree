
var api = {};


api.comment_add = function(request_data) {
    return $.ajax({ url: "./api/comment-add/", type: 'post', cache: false, dataType: 'json',
        data: JSON.stringify( request_data ),
        async:true,
    })
}

api.versions_all = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/versions-all/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}

api.groups_all = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/groups-all/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}