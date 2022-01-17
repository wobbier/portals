using System.IO;
using Sharpmake;

[module: Sharpmake.Include("Engine/Engine.sharpmake.cs")]

[Generate]
public class SharpGameProject : BaseGameProject
{
    public SharpGameProject()
        : base()
    {
        Name = "Portals";
    }
}

[Generate]
public class SharpGameSolution : BaseGameSolution
{
    public SharpGameSolution()
        : base()
    {
        Name = "Portals";
    }
}

[Generate]
public class UserSharpmakeProject : SharpmakeProjectBase
{
    public UserSharpmakeProject() : base() { }
}