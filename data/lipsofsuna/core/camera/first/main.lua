local Client = require("core/client/client")
local FirstPersonCamera = require("core/camera/first/first-person-camera")

Client:register_init_hook(26, function()
	local camera = FirstPersonCamera()
	camera:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	camera:set_far(Client.options.view_distance)
	camera:set_fov(1.1)
	camera:set_near(0.1)
	Client.camera_manager:register_camera("first-person", camera)
end)
