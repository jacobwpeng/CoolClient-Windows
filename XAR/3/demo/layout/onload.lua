
local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")

local frameHostWndTemplate = templateMananger:GetTemplate("Demo.Wnd","HostWndTemplate")
if frameHostWndTemplate then  
	local frameHostWnd = frameHostWndTemplate:CreateInstance("MainWnd")
	if frameHostWnd then
		local objectTreeTemplate = templateMananger:GetTemplate("ControlDemo.MainWnd.Tree","ObjectTreeTemplate")
		if objectTreeTemplate then
		
			local uiObjectTree = objectTreeTemplate:CreateInstance("MainWndTree")
			if uiObjectTree then
		
				frameHostWnd:BindUIObjectTree(uiObjectTree)
				frameHostWnd:Create()
			end
		end
	end
else
	XLMessageBox("!!!!!!")
end
