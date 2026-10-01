@ Route 218, between Jubilife City and Canalave City, laid out after
@ Platinum's on the Rustboro tileset (like Route 104). Crossing it needs Surf.
@ Lines of rocks seal the water off from the top and bottom edges, and
@ trees close the rest of both edges. The east gate building's west wall is
@ an invisible door into the Jubilife gate. The west gate (to Canalave) has
@ the same kind of side door, west-arrow tiles at (6,13)-(6,15), but no warps
@ until it gets an interior.
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
