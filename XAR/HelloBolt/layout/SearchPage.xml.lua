function OnInitControl(self)
	self:SetObjPos(0,0,"father.width","father.height")
end
function OnFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		--XLMessageBox("focus")
		parent:SetTextureID("texture.edit.light.bkg")
	else
		parent:SetTextureID("")
	end
end

function OnLogoDown(self)
	local owner = self:GetOwnerControl():GetParent()
	owner:GetControlObject("ctrl"):SetObjPos2(0,0,"father.width","father.height")
	
	local resultPage = owner:GetControlObject("ResultPage")
	if resultPage then
		owner:RemoveChild(resultPage)
	else
		return false
	end
	--将各子控件状态设置为初试
	owner:GetControlObject("edit"):SetText("")
end

function OnSearchBtnClick(self)
	local owner = self:GetOwnerControl():GetParent()
	local resultPage = owner:GetControlObject("ResultPage")
	local edit = self:GetOwnerControl():GetControlObject("edit")
	
	if resultPage then
		--更新搜索结果
	else 
		local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
		local resultPage = objFactory:CreateUIObject("ResultPage", "ResultPage")
		local searchPage = self:GetOwnerControl()
		searchPage:GetControlObject("ctrl"):SetObjPos2(1920,1080,"father.width","father.height")
		searchPage:AddChild(resultPage)
	end
end
