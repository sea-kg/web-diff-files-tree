
var api = {};

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

api.groups = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/groups/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}

api.files = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/files/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}

api.diff = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/diff/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}

api.comment_add = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/comment-add/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}

api.comment_hide = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/comment-hide/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}

api.add = function(request_data) {
    request_data = request_data || {}
    return $.ajax({
        type: 'post',
        url: "./api/add/",
        data: JSON.stringify(request_data),
        cache: false,
        contentType: "application/json",
        dataType: 'json',
        async:true,
    })
}