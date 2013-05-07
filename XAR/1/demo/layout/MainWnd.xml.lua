

function OnMainWndSize(self,type_, width, height)
	--if type_ == "min" then
	--	return
	--elseif type_ == "max" then
	--
	--elseif type_ == "restored" then
	--
	--end

	local objectTree = self:GetBindUIObjectTree()
	local rootObject = objectTree:GetRootObject()
	
	rootObject:SetObjPos(0, 0, width, height)
end

function OnClickClose(self)
	os.exit(0)
end

function OnClickTextLink(self)
	XLMessageBox("但我们不负责维护哦亲!")
end