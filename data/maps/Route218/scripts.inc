@ Route 218, between Jubilife City and Canalave City, laid out tile for tile
@ after Platinum's (64x32) on the Rustboro tileset. Crossing it needs Surf.
@ The lakes along the top and bottom edges are scenery the player can't
@ reach. The east gate building's west wall is an invisible door into the
@ Jubilife gate, at Platinum's gate warps (57,22)-(57,23). The west gate (to
@ Canalave) has the same kind of side door, west-arrow tiles at
@ (6,18)-(6,19), but no warps until it gets an interior.
Route218_MapScripts::
	.byte 0

Route218_EventScript_RouteSignWest::
	msgbox Route218_Text_RouteSignWest, MSGBOX_SIGN
	end

Route218_EventScript_RouteSignEast::
	msgbox Route218_Text_RouteSignEast, MSGBOX_SIGN
	end

Route218_Text_RouteSignWest:
	.string "ROUTE 218\n"
	.string "{LEFT_ARROW} CANALAVE CITY$"

Route218_Text_RouteSignEast:
	.string "ROUTE 218\n"
	.string "{RIGHT_ARROW} JUBILIFE CITY$"
